#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QMap>

#include "operation.h"
#include "image.h"

class Pipeline
{
    public:

        enum OutputIntent
        {
            LibraryPreview = 1,
            DevelopPreview,
            FinalOutput
        };

        Pipeline();
        ~Pipeline();

        void setIntent(OutputIntent intent);

        void setSource(Image* source);
        void setDestination(Image* destination);

       void execute();

    private:

        OutputIntent         mIntent;
        QMap<int, Operation> mOperations;
        Image*               mSource;
        Image*               mDestination;
};

#endif // PIPELINE_H
