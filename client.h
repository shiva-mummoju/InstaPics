#include"server.h"

int user_id;

void print_header(){
	system("cls");
	printf("\t\t/****** Social Media Application *****/\n\n\n");
}

void main_menu_options(){
	printf("\n\n");
	printf("\t1: Login\n");
	printf("\t2: Sign up\n");
	printf("\t3: exit");
}

void print_new_line(){
	printf("\n\n");
}



void view_post(){
	print_header();
	print_new_line();
	print_all_users();
	char* buff = (char*)malloc(sizeof(char)*20);
	printf("enter the username: ");
	scanf("%s", buff);
	show_post_user(buff);
	print_new_line();
	printf("Enter the post name: ");
	scanf("%s", buff);
	
	print_new_line();
	printf("1. download\n2. Comment\n3. Like\n\n");
	printf("Enter choice: ");
	int choice = 0;
	scanf("%d", &choice);
	if (choice == 1){
		//dowload

	}
	else if (choice == 2){
		//comment

	}
	else if (choice == 3){
		//like

	}
}



void landing_page(){
	print_header();
	printf("\n\n");
	printf("1: view post\n");
	printf("2. upload post\n");
	printf("3: logout\n");
	int choice = 0;
	printf("\n\nEnter choice: ");
	scanf("%d", &choice);
	if (choice == 1){
		view_post();
	}
	else if (choice == 2){
		
		printf("Enter the username: ");
		add_post();
	}
	else if (choice == 3){
		logout();
	}
}

void print_login_page(){
	print_header();
	print_new_line();
	char username[20] , password[20];
	printf("Enter usernae: ");
	scanf("%s", username);
	printf("Enter password: ");
	scanf("%s", password);
	user_id = login(username, password);
	if (user_id>0){
		landing_page();

	}
	else{
		return;
	}

}

void sign_up(){
	print_header();
	char username[20];
	char password[20];
	printf("\n\n\tEnter username: ");
	scanf("%s", username);
	printf("\n\n\tEnter password: ");
	scanf("%s", password);
	create_user( username, password);

}

void run_application(){
	while (true){
		system("cls");
		main_menu_options();
		int choice;
		scanf("%d", &choice);
		if (choice == 1){
			print_login_page();
		}
		else if (choice == 2){
			sign_up();
			system("cls");
		}
		else {
			return;
		}

	}
}