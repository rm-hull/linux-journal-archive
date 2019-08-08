use book_d
go

print "Creating table user_t"
go


if exists(select 1 from sysobjects where name = "user_t")
begin
  drop table user_t
end
go

create table user_t (
   user_id      char(8)      not null,
   password     char(8)      not null,
   first_nm     varchar(30)  not null,
   last_nm      varchar(40)  not null,
   address      varchar(40)  not null,
   city         varchar(40)  not null,
   state        char(2)      not null,
   zipcode      char(9)      not null,
   phone_nbr    char(10)     not null
)
go

execute sp_primarykey user_t, user_id
go

