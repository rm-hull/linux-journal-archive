use book_d
go

print "Creating procedure place_order_pr"
go

if exists(select 1 from sysobjects where name = "place_order_pr")
begin
    drop proc place_order_pr
end
go

create procedure place_order_pr
  @param_user_id     char(8),
  @param_title_id    char(6),
  @param_qty         integer
as
begin
  declare @order_nbr  numeric(5)

  begin transaction

  /* Remove ordered books from inventory */
  update inventory_t set stock_amt = stock_amt - @param_qty
    where title_id = @param_title_id

  if @@error != 0
  begin
    rollback
    return 0
  end

  /* Create a new unique order number */
  update order_nbr_t set order_nbr = order_nbr + 1
  select @order_nbr = order_nbr from order_nbr_t

  /* Add the order into the order table */
  insert into orders_t (order_nbr, user_id, title_id, qty,ordered_dt) values 
    (@order_nbr, @param_user_id, @param_title_id, @param_qty,getdate())

  if @@error != 0
  begin
    rollback
    return 0
  end

  commit
  return 1
end
go

grant execute on place_order_pr to store_user
go

