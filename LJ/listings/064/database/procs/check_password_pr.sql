use book_d
go

print "Creating procedure check_password_pr"
go


if exists(select 1 from sysobjects where name = "check_password_pr")
begin
    drop proc check_password_pr
end
go

create procedure check_password_pr
  @param_user_id     char(8),
  @param_password    char(8)
as
begin
  declare @p_password  char(8)

  select @p_password = password
    from user_t
   where user_id = @param_user_id

  if @p_password = @param_password
    return 1
  else
    return 0
end
go

grant execute on check_password_pr to store_user
go

