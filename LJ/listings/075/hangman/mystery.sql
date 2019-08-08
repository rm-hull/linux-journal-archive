# mSQL statements for inserting mystery words in the database for 
# the hangman program.

# Create mystery words table.
create table mystery (word char(20), category char(15))\g


# Insert a few mystery words.
# IMPORTANT: Characters must be lowercase letters or spaces.
insert into mystery values ('elephant', 'animals')\g
insert into mystery values ('rhinoceros', 'animals')\g
insert into mystery values ('giraffe', 'animals')\g
insert into mystery values ('zebra', 'animals')\g
insert into mystery values ('iguana', 'animals')\g
insert into mystery values ('raccoon', 'animals')\g
insert into mystery values ('turkey', 'animals')\g
insert into mystery values ('penguin', 'animals')\g
insert into mystery values ('gone with the wind', 'movies')\g
insert into mystery values ('star wars', 'movies')\g
insert into mystery values ('the ten commandments', 'movies')\g
insert into mystery values ('american beauty', 'movies')\g
insert into mystery values ('fantasia', 'movies')\g
insert into mystery values ('linus torvalds', 'computer people')\g
insert into mystery values ('marjorie richardson', 'computer people')\g
insert into mystery values ('ken thompson', 'computer people')\g
insert into mystery values ('dennis ritchie', 'computer people')\g
insert into mystery values ('richard stallman', 'computer people')\g
insert into mystery values ('james gosling', 'computer people')\g
insert into mystery values ('miguel icaza', 'computer people')\g
