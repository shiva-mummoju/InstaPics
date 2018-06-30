#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ss_malloc.h"

#define DATABASE_FILENAME "database.bin"
#define FILE_OPEN_MODE "rb+"
#define MAX_NUMBER_POSTS_PER_USER 50
#define MAX_NUMBER_COMMENTS_PER_POST 20
#define MAX_COMMENT_SIZE 50
#define MAX_FILENAME_SIZE 20
#define STARTING_POINT_OF_USER 6

short get_global_user_count();
void show_post_user(char* username);

short global_user_count= -1 ;
int global_post_starting_point  = -1 ;
int last_index;

//size = 104
typedef struct
{
	short user_id;        //id to uniquely identify users who commented on my post
	char del;            // del = 0 then comment deleted ,   del > 0 then it is comment size
	char comment[MAX_COMMENT_SIZE];
}comment;

typedef struct
{
	//short postid;                //this id uniquely identifies the post
	short user_id;                //this iunique;ly identifies a user
	short likes;                //no of likes for this post
	char file_name[MAX_FILENAME_SIZE];            //if filename == ""  then this post is deleted , else it consisits of filename
	char post_title[MAX_FILENAME_SIZE];
	int post_offset;                //this offset refers to the position of 
	int used_size;
	int allocated_size;
	comment comments[MAX_NUMBER_COMMENTS_PER_POST];
} post;

typedef struct
{
	short user_id;
	short no_of_post_offsets;
	char user_name[MAX_FILENAME_SIZE];
	char password[MAX_FILENAME_SIZE];
	int user_post_offsets[MAX_NUMBER_POSTS_PER_USER];
}user;

void create_user(char* user_name,char* password){
	

	FILE* f_write = fopen(DATABASE_FILENAME, "rb+");
	
	fread(&global_user_count, sizeof(short), 1,f_write);

	printf("%d\n", ftell(f_write));
	

	printf("global count %d\n", global_user_count);
	fseek(f_write, 0, SEEK_SET);

	global_user_count++;
	fwrite(&global_user_count, sizeof(short), 1, f_write);

	fseek(f_write, ((global_user_count - 1)*sizeof(user) ) + sizeof(short)+ sizeof(int), SEEK_SET);

	user temp_user;
	
	temp_user.user_id = global_user_count;

	strcpy(temp_user.user_name, user_name);
	strcpy(temp_user.password, password);

	temp_user.no_of_post_offsets = 0;
	for (int i = 0; i < 50; i++){
		temp_user.user_post_offsets[i] = -1;
	}

	fwrite(&temp_user, sizeof(user), 1, f_write);

	//free(&temp_user);
	fclose(f_write);
}

void get_global_data(){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fread(&global_user_count, sizeof(short), 1, f);
	printf("global count in file: %d\n", global_user_count);
	fread(&global_post_starting_point, sizeof(int), 1, f);
	printf("post starting point: %d\n", global_post_starting_point);

	fseek(f, 0, SEEK_END);
	last_index = ftell(f);
	fclose(f);

}

void remove_the_global_count(){
	global_user_count = 0;
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	fwrite(&global_user_count, sizeof(short), 1, f);
	fclose(f);
}

void create_file(){
	FILE* f = fopen(DATABASE_FILENAME, "wb");
	fseek(f, 1024 * 1024 * 100 - 1, SEEK_SET);
	char c = 'a';	
	fwrite(&c, sizeof(char), 1, f);

	fclose(f);
}

short get_userid(char* username){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	//FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	short count = 0;

	fread(&count, sizeof(short), 1, f);
	printf("global count in file: %d\n", count);
	///fclose(f);
//	return count;
	fseek(f, STARTING_POINT_OF_USER, SEEK_SET);
	user u;
	for (short i = 0; i < count; i++){
		fread(&u, sizeof(user), 1, f);
		if (!strcmp(u.user_name, username)){
			fclose(f);
			short temp = u.user_id;
			return temp;
		}
	}
	return -1;
}

int find_the_free_block(int size_of_file){
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	fseek(f, global_post_starting_point, SEEK_SET);
	post p;
	while (true){
		if (ftell(f) + sizeof(post) >= last_index){
			return -1;
		}
		fread(&p, sizeof(post), 1, f);

		if (p.used_size == -1 && size_of_file <= p.allocated_size){
			int p = sizeof(post);
			fseek(f, -p, SEEK_CUR);
			return ftell(f);
		}
		else{
			fseek(f, p.allocated_size, SEEK_CUR);
			if (ftell(f) >= last_index){
				return -1;
			}
		}

	}
}

void add_post(char* username , char* filename, char* post_title){


	short user_id = get_userid(username);




	int current_post_offset = 0;

	//copying the file to buff
	FILE* user_file = fopen(filename, "rb");
	fseek(user_file, 0, SEEK_END);
	int size_of_file = ftell(user_file);
	char* buff = (char*)malloc(sizeof(char)*size_of_file);

	fseek(user_file, 0, SEEK_SET);
	fread(buff, sizeof(char), size_of_file, user_file);
	fclose(user_file);
	buff[size_of_file] = '\0';
	
	
	

	//open the db
	FILE* db = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);

	//get the optimal offset anyway

	current_post_offset = find_the_free_block(size_of_file);

	int allocated_size;
	int flag = 0;
	if (current_post_offset > 0){
		 post p; int post_size = sizeof(post);
		fseek(db, current_post_offset, SEEK_SET);
		fread(&p, sizeof(post), 1, db);
		allocated_size = p.allocated_size;
		fseek(db, -post_size, SEEK_CUR);
	}
	else{
		flag = 1;
		current_post_offset = global_post_starting_point - size_of_file - sizeof(post);
		allocated_size = size_of_file;
	}


	printf("Filesize: %d allocated size: %d, currentoffset: %d flag: %d\n", size_of_file, allocated_size, current_post_offset,flag);
	


	//add this post ofset to the user 
	fseek(db, 6, SEEK_SET);
	user u;
	for (short i = 0; i < global_user_count; i++){
		fread(&u, sizeof(user), 1, db);
		if (u.user_id == user_id){
			if (u.no_of_post_offsets == 50){
				fclose(db);
				printf("REached the limit of he posts\n");
				return;
			}
			//write code to add the post here
			u.no_of_post_offsets++;
			for (int j = 0; j < MAX_NUMBER_POSTS_PER_USER; j++){
				if (u.user_post_offsets[j] <0){
					u.user_post_offsets[j] = current_post_offset;
					break;
				}
			}
			int size_of_user = sizeof(user);
			fseek(db, -size_of_user, SEEK_CUR);
			fwrite(&u, sizeof(user), 1, db);
			break;
		}
	}


	post p;
	p.likes = 0;
	p.post_offset = current_post_offset;
	p.used_size = size_of_file ;
	p.allocated_size = allocated_size;
	strcpy(p.post_title, post_title);
	strcpy(p.file_name, filename);
	for (int i = 0; i < MAX_NUMBER_COMMENTS_PER_POST; i++){
		p.comments[i].del = '0';
		p.comments[i].user_id = user_id;
	}

	fseek(db, current_post_offset, SEEK_SET);

	printf("printing this file at %d\n", ftell(db));

	//write this metadata to the place first and then the post
	fwrite(&p, sizeof(post), 1, db);

	//writing the file itslef
	fwrite(buff, sizeof(char), size_of_file, db);

	//update the global offset
	fseek(db, 2, SEEK_SET );


	if (flag == 1){
		global_post_starting_point = current_post_offset;
	}


	fwrite(&global_post_starting_point, sizeof(int), 1, db);

	//remove this offset in the free list

	fclose(db);


	

}

void set_end_point(){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fseek(f, 0, SEEK_END);
	global_post_starting_point = ftell(f);
	last_index = ftell(f);

	printf("global point started to be %d\n", global_post_starting_point);

	fseek(f, 2, SEEK_SET);
	//fread(&global_user_count, sizeof(short), 1, f);
	fwrite(&global_post_starting_point, sizeof(int), 1, f);
	fclose(f);
	return;
}

void print_all_users(){
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	short count = 0;

	fread(&count, sizeof(short), 1, f);

	fseek(f,  + sizeof(int) + sizeof(short) , SEEK_SET);
	user u;
	for (short i = 0; i <= count; i++){
		fread(&u, sizeof(user), 1, f);
		printf("%s\n", u.user_name);
	}

	fclose(f);
}

void print_global_count(){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	short count = 0;

	fread(&count, sizeof(short), 1, f);

	printf("global count in file: %d\n", count);
	fclose(f);


}

short get_global_user_count(){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fread(&global_user_count, sizeof(short), 1, f);
	printf("global count in file: %d\n", global_user_count);
	fclose(f);
	return global_user_count;
}

void display_the_post_at_this_offset(int offset){
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	fseek(f, offset, SEEK_SET);
	post p;
	fread(&p, sizeof(post), 1, f);
	printf("%s\n", p.file_name);
	fclose(f);
}

void show_post_user(char* username){

	int user_id = get_userid(username);

	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);

	fseek(f, 6, SEEK_SET);

	user u;

	for (short i = 0; i < global_user_count; i++){
		fread(&u, sizeof(user), 1, f);
		if (u.user_id == user_id){
			for (int j = 0; j < u.no_of_post_offsets;j++){
				if (u.user_post_offsets[j] > 0){
					display_the_post_at_this_offset( u.user_post_offsets[j]);
				}
			}
		}
	}

	fclose(f);

}

int download_this_file_name_check(int offset,char* source,char* dest){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fseek(f, offset, SEEK_SET);
	post p;
	fread(&p, sizeof(post), 1, f);
	if (strcmp(p.file_name, source)){
		return 0;
	}
	else{
		FILE* dest_file = fopen(dest, "w");
		int file_size = p.used_size;
		char* filedata = (char*)malloc(sizeof(char)*file_size);
		fread(filedata, sizeof(char), file_size, f);
		fwrite(filedata, sizeof(char), file_size, dest_file);
		fclose(dest_file);
		fclose(f);
		return 1;
	}
}

void download_file(char* username, char* source, char* dest){
	int user_id = get_userid(username);
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);

	fseek(f, 6, SEEK_SET);

	user u;

	for (short i = 0; i < global_user_count; i++){
		fread(&u, sizeof(user), 1, f);
		if (u.user_id == user_id){
			for (int j = 0; j < u.no_of_post_offsets; j++){
				if (u.user_post_offsets[j] > 0){
					if (download_this_file_name_check(u.user_post_offsets[j], source, dest)){
						return;
					}

				}
			}
		}
	}

	fclose(f);

}

int get_post_offset( char* postname){
	user u;
	post p;
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	FILE* only_read = fopen(DATABASE_FILENAME, "rb");
	fseek(f, STARTING_POINT_OF_USER, SEEK_SET);
	for (int i = 0; i < global_user_count; i++){
		fread(&u, sizeof(user), 1, f);
		//int before_loop = ftell(f);
		for (int j = 0; j < u.no_of_post_offsets; j++){
			if (u.user_post_offsets[j] > 0){
				//int before = ftell(f);
				
				fseek(only_read, u.user_post_offsets[j], SEEK_SET);
				//fseek(f, u.user_post_offsets[j], SEEK_SET);
				fread(&p, sizeof(post), 1, only_read);
				if (!strcmp(p.file_name , postname)){
					fclose(f);
					fclose(only_read);
					return u.user_post_offsets[j];
				}
			//	fseek(f, before, SEEK_SET);
				
			}
		}
	}
	fclose(f);
	return -1;

}

void delete_comment(int comment_id, char* postname){
	int post_offset = get_post_offset(postname);

	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fseek(f, post_offset, SEEK_SET);

	post p;
	fread(&p, sizeof(post), 1, f);
	p.comments[comment_id].del = '0';
	int pagesize = sizeof(post);
	fseek(f, -pagesize, SEEK_CUR);
	fwrite(&p, sizeof(post), 1, f);
	fclose(f);

}

void view_comment(char* postname){
	int post_offset = get_post_offset(postname);

	FILE* f = fopen(DATABASE_FILENAME, "rb");
	fseek(f, post_offset, SEEK_SET);

	post p;
	fread(&p, sizeof(post), 1,f);
	for (int i = 0; i < MAX_NUMBER_COMMENTS_PER_POST; i++){
		if (p.comments[i].del == '1'){
			printf("%d  %d %s\n", p.comments[i].user_id, i, p.comments[i].comment);
		}
	}
}

int add_comment_to_post(char* post_name,char* username ,char* comment_data){

	int user_id = get_userid(username);

	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);

	int post_offset = get_post_offset(post_name);

	fseek(f, post_offset, SEEK_SET);

	post p;

	fread(&p, sizeof(post), 1, f);

	for (int i = 0; i < MAX_NUMBER_COMMENTS_PER_POST; i++){
		if (p.comments[i].del == '0'){
			p.comments[i].del = '1';
			strcpy(p.comments[i].comment, comment_data);

			p.comments[i].user_id = user_id;

			int post_size = sizeof(post);
			fseek(f, -post_size, SEEK_CUR);
			fwrite(&p, sizeof(post), 1, f);
			fclose(f);
			return 1;
		}
	}
	fclose(f);
	return 0;
	

}

void delete_post(char* username, char* post_name){
	int post_offset = get_post_offset(post_name);

	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	post p;
	user u;
	int s =  sizeof(user);
	fseek(f, 6, SEEK_SET);
	for (int i = 0; i < global_user_count; i++){
		fread(&u, sizeof(user), 1, f);
		if (!strcmp(u.user_name, username)){
			for (int j = 0; j < u.no_of_post_offsets; j++){

				if (u.user_post_offsets[j] == post_offset){
					
					u.user_post_offsets[j] = -1;
					u.no_of_post_offsets--;
					fseek(f, -s, SEEK_CUR);
					fwrite(&u, sizeof(user), 1, f);
					
					fseek(f, post_offset, SEEK_SET);

					fread(&p, sizeof(post), 1, f);
					p.used_size = -1;
					int post_size = sizeof(post);
					fseek(f, -post_size, SEEK_CUR);
					fwrite(&p, sizeof(post), 1, f);
					fclose(f);
					return;
				}
			}
		}
	}



	fclose(f);
}

void init(){

//	FILE* f = fopen(DATABASE_FILENAME, "wb");
	//fseek(f, 1024 * 1024 * 100 - 1, SEEK_SET);
	//char c = 'a';

	//fwrite(&c, sizeof(char), 1, f);

	//fclose(f);
	//set_end_point();

	FILE* fa = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	//int temp;
	fseek(fa, 2, SEEK_SET);
	fread(&global_post_starting_point, sizeof(int), 1, fa);
	//	printf("The global post count fount to be %d\n", global_post_starting_point);


	fclose(fa);
	/*printf("\nadding users\n");
	create_user("shiva");
	create_user("somesh");
	create_user("abhinav");

	printf("\nadding two posts\n");

	add_post("somesh", "test.txt");
	add_post("somesh", "test1.txt");

	printf("\ndisplaying the posts of somesh\n");
	show_post_user("somesh");

	printf("\ndownloading the file\n");
	download_file("somesh", "test.txt", "destination.txt");

	printf("\nadd comment to test.txt\n");
	add_comment_to_post("test.txt", "shiva", "this is really wonderfull");

	printf("\n viewing the comments of test.txt\n");
	view_comment("test.txt"); 

	printf("\n deleting comment \n");
	delete_comment(0, "test.txt");

	printf("\n viewing the comments of test.txt\n");
	view_comment("test.txt");


	
	printf("\ndisplaying the posts of somesh\n");
	show_post_user("somesh");

	printf("\n adding test1 again  \n");
	add_post("somesh", "test3.txt");


	printf("\ndisplaying the posts of somesh\n");
	show_post_user("somesh");

	printf("deleting test1");
	delete_post("somesh" ,"test1.txt");


	printf("\ndisplaying the posts of somesh\n");
	show_post_user("somesh");

	printf("\n adding test4  \n");
	add_post("somesh", "test4.txt");


	printf("\ndisplaying the posts of somesh\n");
	show_post_user("somesh");
*/

}

void start(){
	get_global_data();
}

void add_like(char* post_name){
	int page_offset = get_post_offset(post_name);

	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);
	fseek(f, 6, SEEK_SET);
	user u; post p;
	for (int i = 0; i < global_user_count; i++){
		for (int j = 0; j < u.no_of_post_offsets; j++){
			if (u.user_post_offsets[j] == page_offset){
				fseek(f, u.user_post_offsets[j], SEEK_SET);
				fread(&p, sizeof(p), 1, f);
				p.likes++;
				int post_size = sizeof(post);
				fwrite(&p, sizeof(p), 1, f);
				fclose(f);
				return;
			}
		}
	}
	fclose(f);
}

int login(char* username, char* password){
	FILE* f = fopen(DATABASE_FILENAME, FILE_OPEN_MODE);;
	fseek(f, 6, SEEK_SET);
	user u;
	for (int i = 0; i < global_user_count; i++){
		if (!strcmp(u.user_name, username) && !strcmp(u.password, password)){
			return u.user_id;
		}
	}
	return -1;
}