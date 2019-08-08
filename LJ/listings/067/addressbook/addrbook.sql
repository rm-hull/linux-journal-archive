create sequence id;

create table session
  (session_id varchar(256) primary key,
   username varchar(32) not null,
   password varchar(256) not null,
   expires int not null);

create table luser
   (uid int primary key default nextval('id'),
    ulogin varchar(15) not null,
    upass varchar(15) not null,
    fname varchar(15) not null,
    lname varchar(20),
    email varchar(90) not null,
    privlevel int not null default 0);

create table person
   (pid int primary key default nextval('id'),
    uid int not null references luser,
    fname varchar(15) not null,
    minit varchar(5),
    lname varchar(20),
    dyn_id varchar(10));

create table email
   (eid int primary key default nextval('id'),
    pid int not null references person,
    address varchar(90) not null);

create table www_url
   (wid int primary key default nextval('id'),
    pid int not null references person,
    url varchar(90) not null);

create table telephone
   (tid int primary key default nextval('id'),
    pid int not null references person,
    countrycode varchar(5) default '1',
    areacode varchar(5),
    phone varchar(10),
    ext varchar(10));

create table country
   (cid int primary key default nextval('id'),
    cname varchar(20) not null);

create table state
   (sid int primary key default nextval('id'),
    cid int references country,
    sname varchar(20) not null);

create table post_main
   (aid int primary key default nextval('id'),
    pid int not null references person,
    city varchar(30),
    sid int references state,
    cid int references country,
    postcode varchar(15));

create table post_lines
   (plid int primary key default nextval('id'),
    pmid int references post_main,
    ltext varchar(90));

-- a view representing a postal address
-- we will want to order by uid, pid, aid, plid;

create view post_addresses as
      select luser.uid as user_id, 
             person.pid as person_id, 
             aid, plid, 
             person.fname as first_name, 
             person.minit as middle, 
             person.lname as last_name, 
             dyn_id, 
             ltext as address_line,
             city, 
             sname as state,
             cname as country, 
             postcode as postal_code
        from luser, person, post_main,
             post_lines, state,
             country
       where luser.uid = person.uid and
             post_main.pid = person.pid and
             post_lines.pmid = post_main.aid and
	     state.sid = post_main.sid and
             country.cid = post_main.cid;

-- this function makes a pretty phone number from
-- countrycode, areacode, phonenumber
-- in the format "+ xxx (yyy) zzzzzzzz"
-- where xxx is countrycode
--       yyy is areacode
--   and zzz is the phonenumber

create function mkphone(varchar, varchar, varchar) returns varchar
    as 'select (''+'' || ($1  || ('' ('' || ( $2 || ( '') '' || $3)))));'
  language 'sql';

create view phone_numbers as
      select luser.uid as user_id,
             tid as pnum_id,
             person.pid as person_id, 
             person.fname as first_name, 
             person.minit as middle, 
             person.lname as last_name, 
             mkphone(countrycode, areacode, phone) as phone_num,
             ext
        from luser, person, telephone
       where luser.uid = person.uid and
             telephone.pid = person.pid;

create function mkmailto(varchar) returns varchar
    as 'select ''<a href="mailto:'' || ($1 || (''">'' || ($1 || ''</a>'')));'
language 'sql';

create view email_addresses as
      select luser.uid as user_id,
             eid as eaddr_id,
             person.pid as person_id, 
             person.fname as first_name, 
             person.minit as middle, 
             person.lname as last_name, 
             address as email,
             mkmailto(address) as mailto_url
        from luser, person, email
       where luser.uid = person.uid and
             email.pid = person.pid;

create function mkhref(varchar) returns varchar
    as 'select ''<a href="'' || ($1 || (''">'' || ($1 || ''</a>'')));'
language 'sql';

create view www_addresses as
      select luser.uid as user_id,
             wid as www_id,
             person.pid as person_id,
             url as url_text,
             mkhref(url) as href
        from luser, person, www_url
       where luser.uid = person.uid and
             www_url.pid = person.pid;

-- you will probably want to change this in a production 
--  environment by creating a new db user
grant all on 
	country, email, email_addresses, id, luser, person, 
	phone_numbers, post_addresses, post_lines, post_main, 
	state, telephone, www_addresses, www_url 
   to public;