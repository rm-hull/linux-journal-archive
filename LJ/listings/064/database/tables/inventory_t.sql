use book_d
go

print "Creating table inventory_t (You may receive an error message)" 
go

execute sp_droptype tid
go

execute sp_addtype tid, "varchar(6)", "not null"
go

if exists(select 1 from sysobjects where name = "inventory_t")
begin
  drop table inventory_t
end
go

create table inventory_t (
    title_id    tid         not null,
    stock_amt   numeric(5)  not null,
    order_amt   numeric(5)  not null
)
go

execute sp_primarykey inventory_t, title_id
go

insert into inventory_t (title_id, stock_amt, order_amt)
   select title_id, 10, 0 from pubs2..titles
go

grant select on inventory_t to store_user
go

