#include <QDebug>
#include <QThread>
#include "constants.h"
#include "imagefileloader.h"
#include "import/exivfacade.h"

#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))
#define clip(v,x,y) (max(x,min(v,y)))

CameraMetaData* Metadata::mMetaData = NULL;
CameraMetaData* Metadata::metaData()
{
    if (mMetaData == NULL)
    {
        try {
            mMetaData = new CameraMetaData("/Users/jaapg/Development/PhotoStage/PhotoStage/external/rawspeed/data/cameras.xml");
        } catch (CameraMetadataException &e) {
            // Reading metadata failed. e.what() will contain error message.
            qDebug() << "Error reading metadata" << e.what();
        }
    }
    return mMetaData;
}

ImageFileLoader::ImageFileLoader(const QString& path,const QModelIndex& index)
{
    mModelIndex = index;
    mPath = path;
    //qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

ImageFileLoader::~ImageFileLoader()
{
    //qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
}

int rawspeed_get_number_of_processor_cores()
{
    return 1;
}

void ImageFileLoader::run()
{
    // TODO: catch errors and emit error(QString)
    QImage image;
    QImage pixmap = QImage(mPath);
    if (pixmap.isNull())
    {
        qDebug() << "QImage can't read." << mPath<< "Attempting as raw";
        image = loadRaw();
        if (image.isNull())
            qDebug() << "raw image loading failed";
        pixmap = image;
    } else
    {
        image = pixmap;
    }

    emit dataReady(mModelIndex,image);
}

/**
 * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
 * @param src the input matrix
 * @param dst (out parameter) the result
 * @return true, if successful, false if the inverse doesn't exist
 */
bool ImageFileLoader::compute_inverse(const float src[9], float dst[9])
{
    bool result = false;

#define SRC(x,y) (src[x]*src[y])
    memset(dst,0,9*sizeof(float));

    // See http://en.wikipedia.org/wiki/Invertible_matrix for algorithm
    float determinant;
    float A,B,C,D,E,F,G,H,I;
    A = SRC(4,8)-SRC(5,7);
    B = -(SRC(3,8)-SRC(5,6));
    C = SRC(3,7)-SRC(4,6);
    D = -(SRC(1,8)-SRC(2,7));
    E = SRC(0,8)-SRC(2,6);
    F = -(SRC(0,7)-SRC(1,6));
    G = SRC(1,5)-SRC(2,4);
    H = -(SRC(0,5)-SRC(2,3));
    I = SRC(0,4)-SRC(1,3);

    determinant = src[0]*A + src[1]*B+src[2]*C;
    if (determinant == 0)
    {
        result = false;
    }
    else
    {
        float t= 1.0/determinant;
        dst[0] = t*A;
        dst[1] = t*D;
        dst[2] = t*G;
        dst[3] = t*B;
        dst[4] = t*E;
        dst[5] = t*H;
        dst[6] = t*C;
        dst[7] = t*F;
        dst[8] = t*I;
        result = true;
    }
    return result;
}

void ImageFileLoader::dump_matrix(const QString &name,float m[9])
{
    QString space = QString(name.length(),' ');
    qDebug() << space << "┌";
    qDebug() << space << "│" << m[0] <<" "<<m[1] <<" "<<m[2];
    qDebug() << name << "│" << m[3] <<" "<<m[4] <<" "<<m[5];
    qDebug() << space << "│" << m[6] <<" "<<m[7] <<" "<<m[8];
    qDebug() << space << "└";

}

int ImageFileLoader::compute_cct(float R,float G, float B)
{
    // see here for more details
    // http://dsp.stackexchange.com/questions/8949/how-do-i-calculate-the-color-temperature-of-the-light-source-illuminating-an-ima
    float X, Y, Z;
    X=(-0.14282)*(R)+(1.54924)*(G)+(-0.95641)*(B);
    Y=(-0.32466)*(R)+(1.57837)*(G)+(-0.73191)*(B);
    Z=(-0.68202)*(R)+(0.77073)*(G)+(0.56332)*(B);
    float x, y;
    x=X/(X+Y+Z);
    y=Y/(X+Y+Z);
    float n;
    n=(x-0.3320)/(0.1858-y);
    int CCT=449*n*n*n+3525*n*n+6823.3*n+5520.33;

    return CCT;

}

void ImageFileLoader::mmultm(float *A, float *B, float * out)
{
    out[0] = A[0]*B[0]+A[1]*B[3]+A[2]*B[6];
    out[1] = A[0]*B[1]+A[1]*B[4]+A[2]*B[7];
    out[2] = A[0]*B[2]+A[1]*B[5]+A[2]*B[8];

    out[3] = A[3]*B[0]+A[4]*B[3]+A[5]*B[6];
    out[4] = A[3]*B[1]+A[4]*B[4]+A[5]*B[7];
    out[5] = A[3]*B[2]+A[4]*B[5]+A[5]*B[8];

    out[6] = A[6]*B[0]+A[7]*B[3]+A[8]*B[6];
    out[7] = A[6]*B[1]+A[7]*B[4]+A[8]*B[7];
    out[8] = A[6]*B[2]+A[7]*B[5]+A[8]*B[8];
}

void ImageFileLoader::vmultm(float *V,float *M,float *out)
{

}


QImage ImageFileLoader::loadRaw()
{
    QImage image;

    ExivFacade *ex = ExivFacade::createExivReader();

    ex->openFile(mPath);
    ExifInfo ex_info = ex->data();
    delete(ex);

    FileReader reader(strdup(mPath.toLocal8Bit().data()));
    FileMap * map;
    try {
        qDebug() << "loadRaw() opening" << reader.Filename();
        map = reader.readFile();
    }
    catch (FileIOException &e)
    {
        qDebug() << "Error reading raw" <<e.what();
        return image;
    }

    RawDecoder *decoder;
    try{
        RawParser parser(map);
        decoder = parser.getDecoder();
    }
    catch(RawDecoderException &e)
    {

    }
    decoder->failOnUnknown = 0;
    decoder->checkSupport(Metadata::metaData());

    decoder->decodeRaw();
    decoder->decodeMetaData(Metadata::metaData());
    RawImage raw = decoder->mRaw;
    //raw->scaleBlackWhite();
    float bl = (float)raw->blackLevel;
    float wp = (float)raw->whitePoint;

    //qDebug() << "Blacklevel" <<bl;
    //qDebug() << "Whitepoint" <<wp;

    int components_per_pixel = raw->getCpp();
    int bytes_per_pixel = raw->getBpp();
    RawImageType type = raw->getDataType();
    bool is_cfa = raw->isCFA;

    if (is_cfa && components_per_pixel==1 && type == TYPE_USHORT16 && bytes_per_pixel==2)
    {
        //qDebug() << "run standard demosiac";
        ColorFilterArray cfa = raw->cfa;
        uint32_t cfa_layout = cfa.getDcrawFilter();
        uint16_t horz, vert;
        if (cfa_layout == 0x94949494)
            horz = vert = 0;
        else if (cfa_layout == 0x49494949) {
            horz = 0; vert = 1;
        }

        qDebug() << "dcraw filter:" << QString("%1").arg(cfa_layout,0,16);

        int cfa_width = cfa.size.x;
        int cfa_height = cfa.size.y;
        CFAColor c = cfa.getColorAt(0,0);

        uint16_t * data = (uint16_t *)raw->getData(0,0);
        int width = raw->dim.x;
        int height = raw->dim.y;
        int pitch_in_bytes = raw->pitch;

        width-=2; // cut of the borders off for simplicity.
        height-=2;
        pitch_in_bytes/=2;

        // demosaic the image
        uint8_t *rawimage = new uint8_t[width*height*4];
        memset(rawimage,0,sizeof(uint8_t) * width* height * 4);

        float *work = new float[width*height*4];
        memset(work,0,sizeof(float) * width* height*4);

        float wbr = ex_info.rgbCoeffients[0];
        float wbg = ex_info.rgbCoeffients[1];
        float wbb = ex_info.rgbCoeffients[2];
        qDebug() << "WB coeffs" << wbr << ","<< wbg << ","<< wbb;

        // canon 350D

        float canon350d[9] = { 6018,-617,-965,-8645,15881,2975,-1530,1719,7642 };
        float powershots30[9] = { 10566,-3652,-1129,-6552,14662,2006,-2197,2581,7670 };
        float canon5DMarkII[9] = { 4716,603,-830,-7798,15474,2480,-1496,1937,6651 };

        float raw_xyzd65[9];
        if (ex_info.model == "EOS 350D DIGITAL") {
            //            qDebug() << "inverse for 350d";
            for (int i=0;i<9;i++)
                canon350d[i] /= 10000;
            compute_inverse(canon350d,raw_xyzd65);
        }
        else if (ex_info.model == "PowerShot S30"){
            //            qDebug() << "inverse for S30";
            for (int i=0;i<9;i++)
                powershots30[i] /= 10000;
            compute_inverse(powershots30,raw_xyzd65);
        }
        else if (ex_info.model == "EOS 5D Mark II") {
            //            qDebug() << "inverse for 5D Mark II";
            for (int i=0;i<9;i++)
                canon5DMarkII[i] /= 10000;
            compute_inverse(canon5DMarkII,raw_xyzd65);
        }
        //dump_matrix("raw_xyzd65",raw_xyzd65);

        float xyz65_srgb[9] = {  3.240481343,  -1.537151516,   -0.498536326,
                                 -0.96925495,	1.875990001,	0.041555927,
                                 0.055646639,  -0.204041338,	1.05731107 };

        //        float xyz50_srgb[9] {
        //            3.1338561, -1.6168667, -0.4906146,
        //            -0.9787684,  1.9161415 , 0.0334540,
        //             0.0719453, -0.2289914 , 1.4052427 };
        //dump_matrix("xyz65_srgb",xyz65_srgb);

        // compute the forward matrix;
        // compute the inverse of the white balance matrix
        float wb_matrix[9] = { wbr, 0,   0,
                               0,   wbg, 0,
                               0,   0,   wbb };
        float wb_inv[9];
        compute_inverse(wb_matrix,wb_inv);

        float forward[9] = { // RGB -> XYZ
                             0.412453, 0.357580, 0.180423 ,
                             0.212671, 0.715160, 0.072169 ,
                             0.019334, 0.119193, 0.950227  };
        //        mmultm(raw_xyzd65,wb_inv,forward);
        // => this forward matrix is working, but brings the colors to D65 not to custom WB
        //dump_matrix("forward",forward);

        //apply matrix to WB coeffs.
        float wbrxyz = wbr * raw_xyzd65[0] + wbg*raw_xyzd65[1] + wbb*raw_xyzd65[2];
        float wbgxyz = wbr * raw_xyzd65[3] + wbg*raw_xyzd65[4] + wbb*raw_xyzd65[5];
        float wbbxyz = wbr * raw_xyzd65[6] + wbg*raw_xyzd65[7] + wbb*raw_xyzd65[8];

        //compute the color temp from that.
        int cct = compute_cct(wbrxyz, wbgxyz, wbbxyz);
        int cct1 = compute_cct(wbr, wbg, wbb);
        qDebug() << "CCT after transform:" << cct <<":"<<cct1;
        //        << wbrxyz<<","<<wbgxyz<<","<<wbbxyz;


        float rm=0,gm=0,bm=0;

        // qDebug() << "Converted RGB MAX" << ex_info.RGB_mean[0] <<","<<ex_info.RGB_mean[1] <<","<<ex_info.RGB_mean[2];

        for (int y=1;y<height-1;y++)
        {
            int line = (y-1) * width * 4;
            uint16_t* row = &data[y*pitch_in_bytes];

            for (int x=1;x<width-1;x++)
            {
                int pix = line + (x-1) * 4;

                uint16_t r,g,b;

                // bilinear interpolation

                if ((x&1) == horz && (y&1) == vert) //red
                {
                    assert(cfa.getColorAt(x,y) == CFA_RED);
                    r = row[x];
                    g = (row[x-1] + row[x+1] + row[x-pitch_in_bytes] + row[x+pitch_in_bytes])/4;
                    b = (row[x-1-pitch_in_bytes] + row[x+1-pitch_in_bytes] + row[x-1+pitch_in_bytes] + row[x+1+pitch_in_bytes])/4;

                }
                else if ((x&1)==((~horz)&1) && (y&1)==((~vert)&1)) //blue
                {
                    assert(cfa.getColorAt(x,y) == CFA_BLUE);

                    r = (row[x-1-pitch_in_bytes] + row[x+1-pitch_in_bytes] + row[x-1+pitch_in_bytes] + row[x+1+pitch_in_bytes])/4;
                    g = (row[x-1] + row[x+1] + row[x-pitch_in_bytes] + row[x+pitch_in_bytes])/4;
                    b = row[x];
                }
                else //green
                {
                    if (cfa.getColorAt(x,y) != CFA_GREEN)
                         qDebug() << "Color:"<<cfa.getColorAt(x,y);
                    assert(cfa.getColorAt(x,y) == CFA_GREEN);

                    g = row[x];

                    if ((y&1)==vert) // red horizontal, blue vert
                    {
                        r = (row[x-1]+row[x+1])/2;
                        assert(cfa.getColorAt(x-1,y) == CFA_RED);
                        assert(cfa.getColorAt(x+1,y) == CFA_RED);
                        b = (row[x-pitch_in_bytes] + row[x+pitch_in_bytes])/2;
                    }
                    else // blue horizontal, red vert
                    {
                        r = (row[x-pitch_in_bytes] + row[x+pitch_in_bytes])/2;
                        //                        if (cfa.getColorAt(x-1,y) != CFA_BLUE)
                        //                        {
                        //                            qDebug() << "Color:"<<cfa.getColorAt(x-1,y);
                        //                            qDebug() << "x,y" << x-1 <<","<<y;
                        //                        }
                        assert(cfa.getColorAt(x-1,y) == CFA_BLUE);
                        assert(cfa.getColorAt(x+1,y) == CFA_BLUE);
                        b = (row[x-1]+row[x+1])/2;
                    }
                }

                // convert data to float. normalized to 1

                float rf = clip((r-bl)/(wp-bl),0,1);
                float gf = clip((g-bl)/(wp-bl),0,1);
                float bf = clip((b-bl)/(wp-bl),0,1);

                float rt,gt,bt;

                // apply white balance
                rt = rf * wbr;
                gt = gf * wbg;
                bt = bf * wbb;

                //#define WORKING
#ifdef WORKING
                rf = rt;
                gf = gt;
                bf = bt;
#else
                // data is now in camera color space


                // apply raw -> xyzd65
                //                rf = rt * raw_xyzd65[0] + gt*raw_xyzd65[1] + bt*raw_xyzd65[2];
                //                gf = rt * raw_xyzd65[3] + gt*raw_xyzd65[4] + bt*raw_xyzd65[5];
                //                bf = rt * raw_xyzd65[6] + gt*raw_xyzd65[7] + bt*raw_xyzd65[8];

                // apply the computed forward matrix
                rf = rt * forward[0] + gt*forward[1] + bt*forward[2];
                gf = rt * forward[3] + gt*forward[4] + bt*forward[5];
                bf = rt * forward[6] + gt*forward[7] + bt*forward[8];

                rt = rf;
                gt = gf;
                bt = bf;

                // apply xyzd65 -> sRGB conversion matrix
                rf = rt * xyz65_srgb[0] + gt*xyz65_srgb[1] + bt*xyz65_srgb[2];
                gf = rt * xyz65_srgb[3] + gt*xyz65_srgb[4] + bt*xyz65_srgb[5];
                bf = rt * xyz65_srgb[6] + gt*xyz65_srgb[7] + bt*xyz65_srgb[8];
#endif
                if (rf > rm) rm =rf;
                if (gf > gm) gm =gf;
                if (bf > bm) bm =bf;

                work[pix] = rf;
                work[pix+1] = gf;
                work[pix+2] = bf;

            }
        }


        float mx = 1.0/max(rm,max(gm,bm));
        //        qDebug() << "max r,g,b" << rm <<","<<gm<<","<<bm;
        // mx = 1.0;

        for (int y=0;y<height;y++)
        {
            for (int x=0;x<width;x++)
            {
                int pix = (y*width+x)*4;
                // squeeze into 255 levels (8bit)
                uint8_t r = work[pix]*mx*255;
                uint8_t g = work[pix+1]*mx*255;
                uint8_t b = work[pix+2]*mx*255;

                rawimage[pix+2] = r;
                rawimage[pix+1] = g;
                rawimage[pix] = b;


                // gamma correction
                float gar = 2.2;//2.22;
                float gag = 2.2;//1.8;
                float gab = 2.2;//2.22;
                rawimage[pix] = (uint8_t)(pow(rawimage[pix]/255.0,1.0/gab)*255);  //blue
                rawimage[pix+1] =(uint8_t)(pow(rawimage[pix+1]/255.0,1.0/gag)*255); // green
                rawimage[pix+2] = (uint8_t)(pow(rawimage[pix+2]/255.0,1.0/gar)*255); //red

                // stretch contrast and brightness
                float ct = 1.8;
                float br = -30;
                uint8_t* bp = &rawimage[pix];
                uint8_t* gp = &rawimage[pix+1];
                uint8_t* rp = &rawimage[pix+2];
                *rp = (uint8_t)max(0,min(ct*(float)*rp+br,255));
                *gp = (uint8_t)max(0,min(ct*(float)*gp+br,255));
                *bp = (uint8_t)max(0,min(ct*(float)*bp+br,255));

                rawimage[pix+3] = 0xff;//xff;
            }
        }

        qDebug()  << "tranfering data to image";
        image = QImage(rawimage,width,height,QImage::Format_RGB32);
        // apply post processing
    }


    delete map;
    delete decoder;

    return image;
}


