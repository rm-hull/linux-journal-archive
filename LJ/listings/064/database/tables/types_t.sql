use book_d
go

print "Creating table types_t"
go

if exists(select 1 from sysobjects where name = "types_t")
begin
  drop table types_t
end
go

create table types_t (
   type_cd   char(12)   not null,
   type_desc varchar(30)  not null
)
go

execute sp_primarykey types_t, type_cd
go

insert into types_t values ('UNDECIDED','Unknown')
insert into types_t values ('business','Business Books')
insert into types_t values ('mod_cook','Modern Cook Books')
insert into types_t values ('trad_cook','Traditional Cook Books')
insert into types_t values ('popluar_comp','Popular Computer Books')
insert into types_t values ('psychology','Psychology')
go

grant select on types_t to store_user
go

