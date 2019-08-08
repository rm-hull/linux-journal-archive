use book_d
go

print "Creating procedure check_title_id_pr"
go

if exists(select 1 from sysobjects where name = "check_title_id_pr")
begin
  drop procedure check_title_id_pr
end
go

create procedure check_title_id_pr
  @param_title_id    char(6)
as
begin
  if exists (select 1 from inventory_t where title_id = @param_title_id)
    return 1
  else
    return 0
end
go

grant execute on check_title_id_pr to store_user
go

