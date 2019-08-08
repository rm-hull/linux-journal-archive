use book_d
go

print "Creating procedure list_types_pr"
go

if exists(select 1 from sysobjects where name = "list_types_pr")
begin
  drop proc list_types_pr
end
go

create procedure list_types_pr
as
begin
  select type_cd, type_desc from types_t order by type_desc
end
go

grant execute on list_types_pr to store_user
go

