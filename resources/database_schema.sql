create table if not exists path (
  id        integer primary key autoincrement,
  directory varchar,
  parent_id integer references path (id),
  lft       integer not null default (0),
  rgt       integer not null default (0)
);

create table if not exists photo (
  id                 integer primary key autoincrement,
  path_id            integer references path (id),
  filename           text    not null,
  iso                integer,
  exposure_time      float,
  focal_length       float,
  datetime_original  text,
  photo_hash         text,
  rating             integer,
  color              integer,
  flag               integer,
  rotation           integer,
  longitude          float,
  latitude           float,
  datetime_digitized text,
  copyright          text,
  artist             text,
  aperture           float,
  flash              boolean,
  lens_name          text,
  make               text,
  model              text,
  width              integer,
  height             integer
);

create table if not exists keyword (
  id        integer primary key autoincrement,
  keyword   varchar not null,
  parent_id integer references keyword (id),
  lft       integer not null default (0),
  rgt       integer not null default (0)
);

create unique index if not exists idx_keyword on keyword (
  keyword,
  parent_id
);

create table if not exists photo_keyword (
  photo_id   integer references photo (id),
  keyword_id integer references keyword (id)
);

create unique index if not exists idx_photo_keyword on photo_keyword (
  photo_id,
  keyword_id
);

create table if not exists collection (
  id        integer primary key autoincrement,
  name      varchar,
  parent_id integer,
  lft       integer,
  rgt       integer
);

create table if not exists collection_photo (
  collection_id integer references collection (id),
  photo_id      integer references photo (id)
);

create table if not exists develop_module (
  id          integer primary key,
  module_name varchar not null
);

insert or ignore into develop_module (id,module_name) values (1,"Raw Module");

create unique index if not exists idx_develop_module on develop_module (
  module_name
);

create table if not exists develop_history (
  id                integer      primary key autoincrement,
  photo_id          integer      references photo (id),
  develop_module_id integer      references develop_module (id),
  friendlytext      varchar      not null,
  is_mutable        integer      not null default(1)
);

create table if not exists develop_setting_raw (
  id                 integer primary key autoincrement,
  develop_history_id         references develop_history (id),
  red_multiplier     integer not null,
  green_multiplier   integer not null,
  blue_multiplier    integer not null,
  rotation           integer not null,
  demosaic_algorithm integer not null
);
