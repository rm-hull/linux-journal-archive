use book_d
go

print "Creating procedure get_book_info_pr"
go

if exists(select 1 from sysobjects where name = "get_book_info_pr")
begin
  drop proc get_book_info_pr
end
go

create procedure get_book_info_pr
  @param_title_id     char(12)
as
begin
  select title,type,type_desc,price,notes,pubdate,stock_amt,order_amt 
    from pubs2..titles t, inventory_t i, types_t y 
   where t.title_id = i.title_id 
     and t.type = y.type_cd 
     and t.title_id = @param_title_id
end
go

grant execute on get_book_info_pr to store_user
go

