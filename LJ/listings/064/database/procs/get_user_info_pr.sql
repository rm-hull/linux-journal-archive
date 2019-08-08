use book_d
go

print "Creating procedure get_user_info_pr"
go

if exists(select 1 from sysobjects where name = "get_user_info_pr")
begin
  drop proc get_user_info_pr
end
go

create procedure get_user_info_pr
  @param_user_id     char(8)
as
begin
  select first_nm, last_nm, address, city, state, zipcode, phone_nbr
    from user_t
   where user_id = @param_user_id
end
go

grant execute on get_user_info_pr to store_user
go

