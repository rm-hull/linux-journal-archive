use book_d
go

print "Create table orders_t"
go

if exists(select 1 from sysobjects where name = "orders_t")
begin
  drop table orders_t
end
go

create table orders_t (
   order_nbr    numeric(5)     not null,
   user_id      char(8)        not null,
   title_id     tid            not null,
   qty          numeric(5)     not null,
   ordered_dt   smalldatetime  not null,
   shipped_dt   smalldatetime  null
)
go

execute sp_primarykey orders_t, order_nbr
go

