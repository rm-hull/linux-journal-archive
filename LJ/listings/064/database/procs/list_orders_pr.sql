use book_d
go

print "Creating procedure list_orders_pr"
go

if exists(select 1 from sysobjects where name = "list_orders_pr")
begin
  drop procedure list_orders_pr
end
go

create procedure list_orders_pr
  @param_user_id    char(8)
as
begin
  select o.order_nbr, o.title_id, t.title, t.price, o.qty, o.ordered_dt, o.shipped_dt 
    from orders_t o, pubs2..titles t
   where o.title_id = t.title_id
     and o.user_id = @param_user_id
   order by ordered_dt
end
go

grant execute on list_orders_pr to store_user
go

