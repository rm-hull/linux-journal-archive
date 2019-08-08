use book_d
go

print "Creating table order_nbr_t"
go


if exists(select 1 from sysobjects where name = "order_nbr_t")
begin
  drop table order_nbr_t
end
go

create table order_nbr_t (
   order_nbr    numeric(5)     not null
)
go

execute sp_primarykey order_nbr_t, order_nbr
go

insert into order_nbr_t values (1)
go

