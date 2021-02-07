#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STRING_SIZE 100

char cinema_f_name_table[] = "cinema.ind",
cinema_f_name[] = "cinema.fl",
hall_f_name_table[] = "hall.ind",
hall_f_name[] = "hall.fl";

FILE* cinemas_table, * cinemas, * halls, * halls_table;



typedef struct type_cinema
{
	char name[STRING_SIZE];
	char address[STRING_SIZE];
	int phone[STRING_SIZE];
} TCinema;

typedef struct type_table_cinema
{
	int id;
	long pos;
	long pos_hall;
	int number_of_halls;
	int next_hall_id;
	bool existence;
} TTableCinema;

typedef struct type_pair_cinema
{
	TTableCinema table;
	TCinema cinema;
} TCinemaPair;

typedef struct type_hall
{
	char name[STRING_SIZE];
	int number_of_seats;
} THall;

typedef struct type_table_hall
{
	int id;
	bool existence;
	long prev;
	long pos;
} TTableHall;

typedef struct type_pair_hall
{
	THall _hall;
	TTableHall table;
} THallPair;

bool check_existence_in_table(int m)
{
	fseek(cinemas_table, 0L, SEEK_SET);
	int x;
	fread(&x, sizeof(x), 1, cinemas_table);
	if (x < m)
		return false;
	else
		return true;
}

TCinemaPair get_m(int m)
{
	--m;
	TCinemaPair ans;
	if (!check_existence_in_table(m))
	{
		ans.table.existence = false;
		return ans;
	}
	fseek(cinemas_table, (long)m * sizeof(TTableCinema) + sizeof(int), SEEK_SET);
	fread(&ans.table, sizeof(TTableCinema), 1, cinemas_table);
	if (ans.table.existence)
	{
		fseek(cinemas, ans.table.pos, SEEK_SET);
		fread(&ans.cinema, sizeof(TCinema), 1, cinemas);
	}
	return ans;
}

bool del_m(int m)
{
	TCinemaPair current = get_m(m);
	if (!current.table.existence)
		return false;
	--m;
	fseek(cinemas_table, (long)m * sizeof(TTableCinema) + sizeof(int), SEEK_SET);
	current.table.existence = false;
	fwrite(&current.table, sizeof(TTableCinema), 1, cinemas_table); 
	return true;
}

THallPair get_s(int s, int m)
{
	THallPair h_pair;
	TCinemaPair c_pair = get_m(m);
	if (!c_pair.table.existence)
	{
		h_pair.table.existence = false;
		return h_pair;
	}
	long pos = (long)c_pair.table.pos_hall;
	int block_size = c_pair.table.next_hall_id - 1;
	if (s > block_size || s <= 0)
	{
		h_pair.table.existence = false;
		return h_pair;
	}

	int id = s + 1;
	while (id > s&& pos != -1)
	{
		fseek(halls_table, pos, SEEK_SET);
		fread(&h_pair.table, sizeof(TTableHall), 1, halls_table);
		id = h_pair.table.id;
		pos = h_pair.table.prev;
	}
	if (s != id)
	{
		h_pair.table.existence = false;
	}
	else
	{
		int pos_in_halls = h_pair.table.pos;
		fseek(halls, pos_in_halls, SEEK_SET);
		fread(&h_pair._hall, sizeof(THall), 1, halls);
	}
	return h_pair;
}

bool del_s(int s, int m)
{
	THallPair h_pair = get_s(s, m);
	if (!h_pair.table.existence)
		return false;
	h_pair.table.existence = false;
	TCinemaPair p_cinema = get_m(m);
	long pos = -2, prev = -1;
	int id = s + 1;
	while (id > s&& pos != -1)
	{
		prev = pos;
		if (pos == -2)
			pos = p_cinema.table.pos_hall;
		else
			pos = h_pair.table.prev;
		fseek(halls_table, pos, SEEK_SET);
		fread(&h_pair.table, sizeof(TTableHall), 1, halls_table);
		id = h_pair.table.id;
	}
	
	fseek(halls_table, sizeof(TTableHall), SEEK_CUR);
	fwrite(&h_pair.table, sizeof(TTableHall), 1, halls_table);

	pos = h_pair.table.prev;
	if (prev > 0)
	{
		fseek(halls_table, prev, SEEK_SET);
		fread(&h_pair.table, sizeof(TTableHall), 1, halls_table);
		h_pair.table.prev = pos;
		fseek(halls_table, sizeof(TTableHall), SEEK_CUR);
		fwrite(&h_pair.table, sizeof(TTableHall), 1, halls_table);
	}
	else
	{
		p_cinema.table.pos_hall = pos;
	}

	--p_cinema.table.number_of_halls;
	if (p_cinema.table.number_of_halls == 0)
	{
		p_cinema.table.pos_hall = -1;
	}
	fseek(cinemas_table, (long)(m - 1) * sizeof(TTableCinema) + sizeof(int), SEEK_SET);
	fwrite(&p_cinema.table, sizeof(TTableCinema), 1, cinemas_table);
	return true;
}

bool update_m(int m)
{
	TCinemaPair current = get_m(m);
	if (!current.table.existence)
		return false;

	printf("Choose what you want to change:\n\t1 - Name;\n\t2 - Address;\n\t3 - Phone");
	int x;
	scanf("%d", &x);
	if (x == 1)
	{
		printf("Enter new value of name:\n");
		scanf("%s", &current.cinema.name);
	}
	else if (x == 2)
	{
		printf("Enter new value of address:\n");
		scanf("%s", &current.cinema.address);
	}
	else if (x == 3)
	{
		printf("Enter new value of phone:\n");
		scanf("%s", &current.cinema.phone);
	}

	long pos = current.table.pos;
	fseek(cinemas, pos, SEEK_SET);
	fwrite(&current.cinema, sizeof(TCinema), 1, cinemas);
	return true;
}

bool update_s(int s, int m)
{
	THallPair h_pair = get_s(s, m);
	if (!h_pair.table.existence)
	{
		return false;
	}
	printf("Choose what you want to change:\n\t1 - Name;\n\t2 - Number of seats;\n");
	int x;
	scanf("%d", &x);
	if (x == 1)
	{
		printf("Enter new value:\n");
		scanf("%s", &h_pair._hall.name);
	}
	else if (x == 2)
	{
		printf("Enter new value:\n");
		scanf("%d", &h_pair._hall.number_of_seats);
	}
	
	long pos = h_pair.table.pos;
	fseek(halls, pos, SEEK_SET);
	fwrite(&h_pair._hall, sizeof(THall), 1, halls);
	return true;
}

bool insert_m()
{
	fseek(cinemas_table, 0L, SEEK_SET);
	int id = 10;
	fread(&id, sizeof(int), 1, cinemas_table);
	//printf("%d\n", id);

	TCinemaPair c_pair;
	c_pair.table.existence = true;
	c_pair.table.id = id;
	c_pair.table.number_of_halls = 0;
	c_pair.table.next_hall_id = 1;
	c_pair.table.pos_hall = -1;
	fseek(cinemas, 0L, SEEK_END);
	fseek(cinemas_table, (long)id * (sizeof(TTableCinema)) + sizeof(int), SEEK_SET);
	c_pair.table.pos = ftell(cinemas);
	fwrite(&c_pair.table, sizeof(TTableCinema), 1, cinemas_table);
	printf("Enter name of the cinema:\n");
	scanf("%s", &c_pair.cinema.name);
	printf("Enter address of  the cinema:\n");
	scanf("%s", &c_pair.cinema.address);
	printf("Enter phone of the cinema:\n");
	scanf("%s", &c_pair.cinema.phone);
	fwrite(&c_pair.cinema, sizeof(TCinema), 1, cinemas);

	fseek(cinemas_table, 0L, SEEK_SET);
	++id;
	fwrite(&id, sizeof(int), 1, cinemas_table);
	return true;
}

bool insert_s(int m)
{
	TCinemaPair c_pair = get_m(m);
	if (!c_pair.table.existence)
	{
		return false;
	}
	THallPair h_pair;
	h_pair.table.existence = true;
	h_pair.table.id = c_pair.table.next_hall_id++;
	fseek(halls, 0L, SEEK_END);
	h_pair.table.pos = ftell(halls);
	h_pair.table.prev = c_pair.table.pos_hall;


	fseek(halls_table, 0L, SEEK_END);
	c_pair.table.pos_hall = ftell(halls_table);
	fwrite(&h_pair.table, sizeof(TTableHall), 1, halls_table);
	++c_pair.table.number_of_halls;

	printf("Enter the name of the hall:\n");
	scanf("%s", &h_pair._hall.name);
	printf("Enter the number of seats of the hall:\n");
	scanf("%d", &h_pair._hall.number_of_seats);
	fwrite(&h_pair._hall, sizeof(THall), 1, halls);
	printf("Id of this hall is: %d\n", h_pair.table.id);

	fseek(cinemas_table, (long)(m - 1) * sizeof(TTableCinema) + sizeof(int), SEEK_SET);
	fwrite(&c_pair.table, sizeof(TTableCinema), 1, cinemas_table);

	return true;
}

int output_all_id()
{
	fseek(cinemas_table, 0L, SEEK_SET);
	int y;
	fread(&y, sizeof(int), 1, cinemas_table);
	return y;
}

void reload_all_files()
{
	FILE* new_cinemas = fopen("cinema.temp_fl", "wb+"),
		* new_cinemas_table = fopen("cinema.temp_ind", "wb+"),
		* new_halls = fopen("halls.temp_fl", "wb+"),
		* new_halls_table = fopen("halls.temp_ind", "wb+");

	int all_id;
	fseek(cinemas_table, 0L, SEEK_SET);
	fread(&all_id, sizeof(int), 1, cinemas_table);
	fseek(new_cinemas_table, 0L, SEEK_SET);
	fwrite(&all_id, sizeof(int), 1, new_cinemas_table);

	TCinemaPair c_pair;
	THallPair h_pair;
	int k = 0, i;
	for (i = 0; i < all_id; ++i)
	{
		fread(&c_pair.table, sizeof(TTableCinema), 1, cinemas_table);
		if (c_pair.table.existence)
		{
			c_pair.table.id = k;
			fseek(cinemas, c_pair.table.pos, SEEK_SET);
			fread(&c_pair.cinema, sizeof(TCinema), 1, cinemas);
			fseek(new_cinemas, 0L, SEEK_END);
			c_pair.table.pos = ftell(new_cinemas);
			fwrite(&c_pair.cinema, sizeof(TCinema), 1, new_cinemas);

			int pos = c_pair.table.pos_hall;
			int c = c_pair.table.number_of_halls;
			int prev = -1;
			while (pos != -1)
			{
				fseek(halls_table, pos, SEEK_SET);
				fread(&h_pair.table, sizeof(TTableHall), 1, halls_table);
				h_pair.table.id = c;
				fseek(halls, h_pair.table.pos, SEEK_SET);
				fread(&h_pair._hall, sizeof(THall), 1, halls);
				fseek(new_halls, 0L, SEEK_END);
				h_pair.table.pos = ftell(new_halls);
				fwrite(&h_pair._hall, sizeof(THall), 1, new_halls);
				pos = h_pair.table.prev;
				h_pair.table.prev = prev;
				fseek(new_halls_table, 0L, SEEK_END);
				prev = ftell(new_halls_table);
				fwrite(&h_pair.table, sizeof(TTableHall), 1, new_halls_table);
				--c;
			}
			c_pair.table.pos_hall = prev;
			c_pair.table.next_hall_id = c_pair.table.number_of_halls + 1;
			fwrite(&c_pair.table, sizeof(TTableCinema), 1, new_cinemas_table);
			++k;
		}
	}
	fseek(new_cinemas_table, 0L, SEEK_SET);
	fwrite(&k, sizeof(int), 1, new_cinemas_table);
	fclose(halls); fclose(halls_table); fclose(cinemas); fclose(cinemas_table);
	cinemas_table = fopen(cinema_f_name_table, "wb+"),
		cinemas = fopen(cinema_f_name, "wb+"),
		halls_table = fopen(hall_f_name_table, "wb+"),
		halls = fopen(hall_f_name, "wb+");
	fclose(halls); fclose(halls_table); fclose(cinemas); fclose(cinemas_table);
	cinemas_table = fopen(cinema_f_name_table, "rb+"),
		cinemas = fopen(cinema_f_name, "rb+"),
		halls_table = fopen(hall_f_name_table, "rb+"),
		halls = fopen(hall_f_name, "rb+");

	all_id = k;
	fwrite(&k, sizeof(int), 1, cinemas_table);
	for (i = 0; i < all_id; ++i)
	{
		fread(&c_pair.table, sizeof(TTableCinema), 1, new_cinemas_table);
		fseek(new_cinemas, c_pair.table.pos, SEEK_SET);
		fread(&c_pair.cinema, sizeof(TCinema), 1, new_cinemas);
		fseek(cinemas, 0L, SEEK_END);
		c_pair.table.pos = ftell(cinemas);
		fwrite(&c_pair.cinema, sizeof(TCinema), 1, cinemas);
		int pos = c_pair.table.pos_hall;
		int prev = -1;
		while (pos != -1)
		{
			fseek(new_halls_table, pos, SEEK_SET);
			fread(&h_pair.table, sizeof(TTableHall), 1, new_halls_table);
			fseek(new_halls, h_pair.table.pos, SEEK_SET);
			fread(&h_pair._hall, sizeof(THall), 1, new_halls);
			fseek(halls, 0L, SEEK_END);
			h_pair.table.pos = ftell(halls);
			fwrite(&h_pair._hall, sizeof(THall), 1, halls);
			pos = h_pair.table.prev;
			h_pair.table.prev = prev;
			fseek(halls_table, 0L, SEEK_END);
			prev = ftell(halls_table);
			fwrite(&h_pair.table, sizeof(TTableHall), 1, halls_table);
		}
		c_pair.table.pos_hall = prev;
		fwrite(&c_pair.table, sizeof(TTableCinema), 1, cinemas_table);
	}
	fclose(new_halls);
	fclose(new_halls_table);
	fclose(new_cinemas);
	fclose(new_cinemas_table);
	if (remove("cinema.temp_fl") == 0 && remove("cinema.temp_ind") == 0 &&
		remove("halls.temp_fl") == 0 && remove("halls.temp_ind") == 0)
		printf("Cleaned\n");
	else
		printf("Some problems in cleaning!");
}

int main()
{
	cinemas_table = fopen(cinema_f_name_table, "rb+"),
		cinemas = fopen(cinema_f_name, "rb+"),
		halls_table = fopen(hall_f_name_table, "rb+"),
		halls = fopen(hall_f_name, "rb+");
	int x = 0;

	while (true)
	{

		printf("In which table do you want to work:\
              \n\t0 - exit;\
              \n\t1 - cinemas;\
              \n\t2 - halls;\
              \n\t3 - clean from garbage\n");
		scanf("%d", &x);
		if (x == 0)
			break;
		if (x == 1)
		{
			printf("----------------------TABLE OF CINEMAS----------------------\n");
			while (true)
			{
				//printf("%d\n", output_all_id());
				printf("Choose what you want to do:\
                    \n\t0 - return to start menu;\
                    \n\t1 - add to table;\
                    \n\t2 - search in table;\
                    \n\t3 - delete from table;\
                    \n\t4 - update cinema info in table.\n");
				scanf("%d", &x);
				if (x == 0)
					break;
				else if (x == 1)
				{
					if (insert_m())
						printf("Successfully added!\n\n");
					else
						printf("Some problems in adding!\n\n");
				}
				else if (x == 2)
				{
					int m;
					printf("Enter id of the cinema:\n");
					scanf("%d", &m);
					TCinemaPair cur_cinema = get_m(m);
					if (!cur_cinema.table.existence)
					{
						printf("Cinema with this id doesn't exist:((\n\n");
					}
					else
					{
						printf("---Info about cinema #%d---\n", m);
						printf("Name: %s\n", cur_cinema.cinema.name);
						printf("Address: %s\n", cur_cinema.cinema.address);
						printf("Phone: %s\n", cur_cinema.cinema.phone);
						printf("Number of halls right now: %d\n", cur_cinema.table.number_of_halls);
					}
				}
				else if (x == 3)
				{
					int m;
					printf("Enter id of the cinema:\n");
					scanf("%d", &m);
					if (!del_m(m))
						printf("Cinema with this id doesn't exist:((\n\n");
					else
						printf("Successfully deleted!\n\n");
				}
				else if (x == 4)
				{
					printf("Enter id of the cinema:\n");
					int m;
					scanf("%d", &m);
					if (!update_m(m))
						printf("Cinema with this id doesn't exist:((\n\n");
					else
						printf("Successfully changed!\n\n");
				}
				else
					printf("Try again:)\n\n");
			}
		}
		else if (x == 2)
		{
			printf("----------------------TABLE OF HALLS----------------------\n");
			while (true)
			{
				printf("Choose what you want to do:\
                    \n\t0 - return to start menu;\
                    \n\t1 - add to table;\
                    \n\t2 - search in table;\
                    \n\t3 - delete from table;\
                    \n\t4 - update case info in table.\n");
				scanf("%d", &x);
				if (x == 0)
					break;
				else if (x == 1)
				{
					int m;
					printf("Enter id of the cinema to which to add the hall:\n");
					scanf("%d", &m);
					if (insert_s(m))
						printf("Successfully added!\n\n");
					else
						printf("Sorry! Some problems in adding!\n\n");
				}
				else if (x == 2)
				{
					int m;
					printf("Enter id of the cinema where to search hall:\n");
					scanf("%d", &m);
					int s;
					printf("Enter id of the hall:\n");
					scanf("%d", &s);

					THallPair h_pair = get_s(s, m);
					if (!h_pair.table.existence)
					{
						printf("Hall with this id doesn't exist:((\n\n");
					}
					else
					{
						printf("---info about hall #%d of cinema #%d---\n", s, m);
						printf("Name: %s\n", h_pair._hall.name);
						printf("Number of seats: %d\n", h_pair._hall.number_of_seats);
					}
				}
				else if (x == 3)
				{
					int m;
					printf("Enter id of the cinema where to delete hall:\n");
					scanf("%d", &m);
					int s;
					printf("Enter id of the hall:\n");
					scanf("%d", &s);
					if (!del_s(s, m))
						printf("Hall with this id doesn't exist:((\n\n");
					else
						printf("Successfully deleted!\n\n");
				}
				else if (x == 4)
				{
					printf("Enter id of the cinema where to update hall:\n");
					int m;
					scanf("%d", &m);
					int s;
					printf("Enter id of the hall:\n");
					scanf("%d", &s);
					if (!update_s(s, m))
						printf("Hall with this id doesn't exist:((\n\n");
					else
						printf("Successfully changed!\n\n");
				}
				else
					printf("Try again:)\n\n");
			}
		}
		else if (x == 3)
		{
			reload_all_files();
		}
		else
			printf("Try again!\n\n");
	}

	fclose(cinemas);
	fclose(cinemas_table);
	fclose(halls);
	fclose(halls_table);
	return 0;
}
