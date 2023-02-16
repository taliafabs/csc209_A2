#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "friends.h"

int find_name(User *my_users, char name[]) {
	char buffer[MAX_NAME];
	FILE *out = freopen("tests_output.txt", "w", stdout);
        list_users(my_users);
        fclose(out);
        freopen("/dev/tty", "w", stdout);
        FILE *result = fopen("tests_output.txt", "r");
        int found = 1;
        while (fgets(buffer, MAX_NAME, result)) {
                if (strcmp(buffer, name) == 0) {
                        found = 0;
                        break;
                }
        }
	return found;
}

int check_friendship(User *my_users, char name1[], char name2[]) {
	int friends = 1;
	User *user1 = find_user(name1, my_users);
	User *user2 = find_user(name2, my_users);
        for (int i = 0; i < MAX_FRIENDS; i++) {
                if (user1->friends[i] != NULL) {
                        if (strcmp(user1->friends[i]->name, name2) == 0) {
                                friends = 0;
                                break;
                        }
                }
        }
	return friends;
}

int main() {
	// Create a string that is too long
	char too_long[MAX_NAME + 5];
	for (int i = 0; i < MAX_NAME + 4; i++) {
		too_long[i] = 'a';
	}
	too_long[MAX_NAME + 4] = '\0';

	// Test the create_user and find_user methods
	User *my_users = NULL;
	assert (create_user("Bob", &my_users) == 0);
	assert (create_user("Bob", &my_users) == 1);
	assert (create_user(too_long, &my_users) == 2);
	assert (find_user("nonexistant", my_users) == NULL);
	assert (strcmp(my_users->name, "Bob") == 0);
	create_user("Alice", &my_users);
	assert (strcmp(my_users->next->name, "Alice") == 0);

	// Get bob and alice for further testing
	User *bob_ptr = find_user("Bob", my_users);
	User *alice_ptr = find_user("Alice", my_users);
	assert (bob_ptr != NULL && alice_ptr != NULL);

	// Test the list_users method
	FILE *out = freopen("tests_output.txt", "w", stdout); // redirect output to file
	list_users(my_users);
	fclose(out);
	freopen("/dev/tty", "w", stdout); // redirect back to standard output
	FILE *result = fopen("tests_output.txt", "r");
	char actual_line[MAX_NAME];
	fgets(actual_line, MAX_NAME, result);
	assert(strcmp(actual_line, "User List\n") == 0);
	fgets(actual_line, MAX_NAME, result);
	assert (strcmp(actual_line, "\tBob\n") == 0);
	fgets(actual_line, MAX_NAME, result);
	assert (strcmp(actual_line, "\tAlice\n") == 0);
	fclose(result);

	// Test the update_pic method
	assert (strlen(alice_ptr->profile_pic) == 0);
	assert (update_pic(alice_ptr, "non_existant.txt") == 1);
	assert (update_pic(alice_ptr, too_long) == 2);
	assert (strlen(alice_ptr->profile_pic) == 0);
	assert (update_pic(alice_ptr, "alien.ascii") == 0);
	assert (strcmp(alice_ptr->profile_pic, "alien.ascii") == 0);

	// Test the make_friends method
	const char alice_name[MAX_NAME] = "Alice";
	const char bob_name[MAX_NAME] = "Bob";
	assert (make_friends(alice_name, "No", my_users) == 4);
	assert (make_friends("Nope", bob_name, my_users) == 4);
	assert (make_friends("I Don't Exist", "Me Neither", my_users) == 4);
	assert (make_friends(bob_name, bob_name, my_users) == 3);
	assert (make_friends("No", "No", my_users) == 4); // largest error code should apply
	assert (make_friends(alice_name, alice_name, my_users) == 3); // same user that exists
	// Generate MAX_FRIENDS friends for a new user, friendliest
	const char friendliest_name[MAX_NAME] = "Friendliest";
	assert (create_user(friendliest_name, &my_users) == 0);
	for (int i = 0; i < MAX_FRIENDS; i++) {
		char temp_name[MAX_NAME];
		sprintf(temp_name, "%d", i);
		assert (create_user(temp_name, &my_users) == 0); // create user named i
		assert (make_friends(friendliest_name, temp_name, my_users) == 0);
	}
	assert (make_friends(friendliest_name, bob_name, my_users) == 2); // friendliest has max friends already
	assert (make_friends(alice_name, friendliest_name, my_users) == 2); // try the other way around too
	assert (make_friends(friendliest_name, friendliest_name, my_users) == 3);
	// at this point, neither alice not bob should have any friends; check to see
	assert (alice_ptr->friends[0] == NULL);
	assert (bob_ptr->friends[0] == NULL);
	// now make bob and alice friends and ensure it works
	assert (make_friends(alice_name, bob_name, my_users) == 0);
	assert (make_friends(alice_name, bob_name, my_users) == 1); // alice and bob are already friends
	assert (alice_ptr->friends[0] == bob_ptr);
	assert (bob_ptr->friends[0] == alice_ptr);

	// Test the print_user function
	assert(print_user(NULL) == 1);
	assert(strlen(bob_ptr->profile_pic) == 0);
	out = freopen("tests_output.txt", "w", stdout); // redirect output to file
        print_user(bob_ptr);
	fclose(out);
        result = fopen("tests_output.txt", "r");
	const int profile_line_len = 100;
	char profile_line[profile_line_len];
	fgets(profile_line, profile_line_len, result);
	assert(strcmp(profile_line, "Name: Bob\n") == 0);
	fgets(profile_line, profile_line_len, result);
	fgets(profile_line, profile_line_len, result); // skip divider
	assert(strcmp(profile_line, "Friends:\n") == 0);
	fgets(profile_line, profile_line_len, result);
	assert(strcmp(profile_line, "Alice\n") == 0);
	fclose(result);
	freopen("/dev/tty", "w", stdout); // redirect back to standard output

	// Test the make_post function
	memset(alice_ptr->profile_pic, 0, sizeof(alice_ptr->profile_pic)); // get rid of alice's profile picture for easier testing
	assert(make_post(NULL, bob_ptr, "") == 2); // one or both users being null results in 2
	assert(make_post(alice_ptr, NULL, "") == 2);
	assert(make_post(NULL, NULL, "") == 2);
	assert(make_post(bob_ptr, find_user(friendliest_name, my_users), "") == 1); // Bob and Friendliest are not friends
	assert(make_post(bob_ptr, bob_ptr, "") == 1); // Users are not friends with themselves
	assert(bob_ptr->first_post == NULL); // failed calls should not give any users posts
	assert(alice_ptr->first_post == NULL);
	assert(find_user(friendliest_name, my_users)->first_post == NULL);
	char *post_contents = malloc(sizeof(char) * 20);
	strncpy(post_contents, "Hello Alice!", 19);
	assert(make_post(bob_ptr, alice_ptr, post_contents) == 0); // should be successful
	Post *created_post = alice_ptr->first_post;
	assert(strcmp(created_post->author, "Bob") == 0);
	assert(strcmp(created_post->contents, "Hello Alice!") == 0);
	assert(created_post->next == NULL);
	assert(bob_ptr->first_post == NULL); // Bob shouldn't have any posts
	assert(create_user("Alex", &my_users) == 0);
	User *alex_ptr = find_user("Alex", my_users);
	assert(alex_ptr != NULL);
	assert(alex_ptr->friends[0] == NULL);
	assert(make_friends("Alice", "Alex", my_users) == 0); // make alice friends with alex
	char *alex_post = malloc(sizeof(char) * 30);
	strncpy(alex_post, "How are you doing?", 30);
	assert(make_post(alex_ptr, alice_ptr, alex_post) == 0); // make alex make a post to alice
	out = freopen("tests_output.txt", "w", stdout);
	print_user(alice_ptr);
	fclose(out);


	freopen("/dev/tty", "w", stdout);

	// Test print_user and make_post methods together
	assert(strlen(alice_ptr->profile_pic) == 0);
	result = fopen("tests_output.txt", "r");
	fgets(profile_line, profile_line_len, result);
	assert(strcmp(profile_line, "Name: Alice\n") == 0);
	fgets(profile_line, profile_line_len, result);
	fgets(profile_line, profile_line_len, result); // ignore divider
        assert(strcmp(profile_line, "Friends:\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "Bob\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "Alex\n") == 0);
	fgets(profile_line, profile_line_len, result);
	fgets(profile_line, profile_line_len, result); // ignore divider
        assert(strcmp(profile_line, "Posts:\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "From: Alex\n") == 0);
        fgets(profile_line, profile_line_len, result);
        fgets(profile_line, profile_line_len, result); // ignore date; we can't really test this
        assert(strcmp(profile_line, "\n") == 0);
        fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "How are you doing?\n") == 0);
        fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "===\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "From: Bob\n") == 0);
	fgets(profile_line, profile_line_len, result);
	fgets(profile_line, profile_line_len, result); // ignore date; we can't really test this
        assert(strcmp(profile_line, "\n") == 0);
	fgets(profile_line, profile_line_len, result);
        assert(strcmp(profile_line, "Hello Alice!\n") == 0);

	// Test the delete_user method
	assert (delete_user("non existent", &my_users) == 1); // returns 1 when given user that doesn't exist

	create_user("Amy", &my_users); // try deleting an entirely new user
	assert(find_name(my_users, "\tAmy\n") == 0); // assert that amy is now in the list
	assert(delete_user("Amy", &my_users) == 0); // we successfully delete amy
        assert(find_name(my_users, "\tAmy\n") == 1); // assert that amy is no longer in the list

	assert(check_friendship(my_users, "Bob", "Alice") == 0); // assert that bob and alice are friends
	assert (delete_user("Alice", &my_users) == 0); // should delete the user called Alice and return 0
	assert(find_name(my_users, "\tAlice\n") == 1); // alice should no longer be in list of names
	assert(check_friendship(my_users, "Bob", "Alice") == 1); // assert that bob and alice are no longer friends

	assert(find_name(my_users, "\tBob\n") == 0); // assert that Bob is in the list of friends
	assert(delete_user("Bob", &my_users) == 0); // successfully delete Bob, first user in list
	assert(find_name(my_users, "\tBob\n") == 1); // assert that Bob is no longer in the friends list

	printf("Passed all tests!\n");
	return 0;
}

