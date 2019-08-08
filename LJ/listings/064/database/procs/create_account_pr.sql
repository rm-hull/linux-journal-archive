use book_d
go

print "Creating procedure create_account_pr"
go

if exists(select 1 from sysobjects where name = "create_account_pr")
begin
  drop proc create_account_pr
end
go

create procedure create_account_pr
  @param_user_id     char(8),
  @param_password    char(6),
  @param_last_nm     char(40),
  @param_first_nm    char(30),
  @param_address     char(40),
  @param_city        char(40),
  @param_state       char(2),
  @param_zipcode     char(9),
  @param_phone_nbr   char(10)
as
begin
  /* Check for duplicate user_id */
  if exists (select 1 from user_t where user_id = @param_user_id)
     return 1

  insert into user_t (user_id, password, last_nm, first_nm, address,
    city, state, zipcode, phone_nbr) values (@param_user_id,
    @param_password, @param_last_nm, @param_first_nm, @param_address,
    @param_city, @param_state, @param_zipcode, @param_phone_nbr)

  return 0
end
go

grant execute on create_account_pr to store_user
go

