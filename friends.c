#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 if successful
 *   - 1 if a user by this name already exists in this list
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int create_user(const char *name, User **user_ptr_add) {

        User *curr = *user_ptr_add;
    // if the given name can't fit the name array return 2
    if(strlen(name) >= MAX_NAME){
        return 2;
    }
    // go thru the LL basically and check that no user with name already exists
    while(curr != NULL){
        if (strcmp(curr->name, name) == 0){
            return 1;
        }
        curr = curr->next;
    }
    

    // now that it's been determined that this User CAN be created:
    User *new_user = malloc(sizeof(User));
    // give it the name
    strcpy(new_user->name, name);
    // initialize its first_post to be NULL
    new_user->first_post = NULL;
    // the next node is NULL as it's the last!
    new_user->next = NULL;

    // if the LL is empty add it first
    if(*user_ptr_add == NULL){
        *user_ptr_add = new_user;
    }
    // else, traverse to the end of the LL and ad the user there
    else{
        curr = *user_ptr_add;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_user;
    }
    return 0;
}


/*
 * Return a pointer to the user with this name in
 * the list starting with head. Return NULL if no such user exists.
 *
 * NOTE: You'll likely need to cast a (const User *) to a (User *)
 * to satisfy the prototype without warnings.
 */
User *find_user(const char *name, const User *head) {
    // start at the first user object in the linked list
    User *current = (User *) head;
    //const User *current = head;
    // check if the name of every user is this name
    while(current != NULL){
        // if it is return a pointer to the user
        if (strcmp(current->name, name) == 0){
            return (User*)current;
        }
        current = current->next;
    }
    // if that never happens, return NULL
    return NULL;
}


/*
 * Print the usernames of all users in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
void list_users(const User *curr) {
    // start at the curr user
    User *curr_user = (User *) curr;
    // keep going through the linked list till the end is reached
    while(curr_user != NULL){
        fprintf(stdout, "%s\n", curr_user->name);
        //print_user(curr);
         curr_user = curr_user->next;
    }		
}


/*
 * Change the filename for the profile pic of the given user.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the file does not exist or cannot be opened.
 *   - 2 if the filename is too long.
 */
int update_pic(User *user, const char *filename) {
    // check that the filename isnt too long
    size_t len = strlen(filename);
    if (len >= MAX_NAME){
        return 2;
    }
    // check that the file exists and can be opened
    FILE *f = fopen(filename, "rb");
    if (f == NULL){
        return 1;
    }
    fclose(f);

    // passed both of those? time to make the change
    strncpy(user->profile_pic, filename, len+1);
    return 0;
}

/*
 * A helper function to check if two users are friends or not.
 * Pre-condition: both users exist
 * Return:
 *      - 0 if they are NOT friends
 *      - 1 otherwise
 */
int check_if_friends(User *user1, User *user2){
    for(int i = 0; i < MAX_FRIENDS; i++){
      if(user1->friends[i] != NULL && user2->friends[i] != NULL){
          if(strcmp(user1->friends[i]->name, user2->name) == 0 || strcmp(user2->friends[i]->name, user1->name) == 0){
    return 1;
        }
      }
    }
    return 0;
}

/*
 * Make two users friends with each other.  This is symmetric - a pointer to
 * each user must be stored in the 'friends' array of the other.
 *
 * New friends must be added in the first empty spot in the 'friends' array.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the two users are already friends.
 *   - 2 if the users are not already friends, but at least one already has
 *     MAX_FRIENDS friends.
 *   - 3 if the same user is passed in twice.
 *   - 4 if at least one user does not exist.
 *
 * Do not modify either user if the result is a failure.
 * NOTE: If multiple errors apply, return the *largest* error code that applies.
 */
int make_friends(const char *name1, const char *name2, User *head) {
    // find the two users
    User *u1 = find_user(name1, head);
    User *u2 = find_user(name2, head);


    // make sure that neither of them is NULL aka they both do exist
    if(u1 == NULL || u2 == NULL){
        return 4;
    }

    // check that the same user has not been passed in twice
    if (strcmp(name1, name2) == 0){
        return 3;
    }

    // check that neither has max friends
    int u1_friends = 0;
    int u2_friends = 0;
    while(u1->friends[u1_friends] != NULL && u1_friends < MAX_FRIENDS){
        u1_friends += 1;
    }
    while(u2->friends[u2_friends] != NULL && u2_friends < MAX_FRIENDS){
        u2_friends += 1;
    }
    if (u1_friends >= MAX_FRIENDS || u2_friends >= MAX_FRIENDS){
        return 2;
    }

    // check that they are not already friends
    // use the helper function check_if_friends to determine this
    int already_friends = check_if_friends(u1, u2);
    if(already_friends != 0){
        return 1;
    }

    // add new friends
    int successful1 = 0;
    int successful2 = 0;

    for(int i1 = 0; i1 < MAX_FRIENDS; i1++){
        if(u1->friends[i1] == NULL){
            u1->friends[i1] = u2;
            successful1 = 1;
            break;
        }
    }

    if (!successful1) {
        // return 2 if they are not already friends but u1 already has max friends
        return 2;
    }

    for (int i2 = 0; i2 < MAX_FRIENDS; i2++){
        if(u2->friends[i2] == NULL){
            u2->friends[i2] = u1;
            successful2 = 1;
            break;
        }
    }

    if (!successful2) {
        // remove u2 from u1's friends list, since we u2 already had max friends and it couldnt be added there
        for (int i1 = 0; i1 < MAX_FRIENDS; i1++) {
            if (u1->friends[i1] == u2) {
                u1->friends[i1] = NULL;
                break;
            }
        }
        return 2; // u2's friends array is full
    }
    return 0;
}


/*
 * Print a user profile.
 * For an example of the required output format, see the example output
 * linked from the handout.
 * Return:
 *   - 0 on success.
 *   - 1 if the user is NULL.
 */
int print_user(const User *user) {
    // check that the user exists
    User *print_me = (User *) user;
    if (print_me == NULL){
        return 1;
    }
    FILE *pfp_file = fopen(print_me->profile_pic, "r");
    if(pfp_file != NULL){
        char line[MAX_NAME];
        while (fgets(line, MAX_NAME, pfp_file) != NULL){
            printf("%s", line);
        }
    }
    printf("\n");
    printf("Name: %s\n", print_me->name);
    printf("------------------------------------------\n");
    printf("Friends:\n");
    // go through the friends of this user
    for(int i = 0; i < MAX_FRIENDS; i++){
        // make sure that the user at the index is not NULL
        if(print_me->friends[i] != NULL){
            // print the name of that friend (who is a User)
            printf("%s\n", print_me->friends[i]->name);
        }
    }
    printf("------------------------------------------\n");
    printf("Posts: \n");
    if(print_me->first_post != NULL){
        Post *curr_post = print_me->first_post;
        while(curr_post != NULL){
            // do stuff to it here!!!!!!
            printf("From: %s\n", curr_post->author);
            printf("Date: %s\n", ctime(curr_post->date));
            printf("%s\n", curr_post->contents);
            curr_post = curr_post->next;
        }
    }
    printf("===\n");
    return 0;
}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * 'contents' is a pointer to heap-allocated memory - you do not need
 * to allocate more memory to store the contents of the post.
 *
 * Return:
 *   - 0 on success
 *   - 1 if users exist but are not friends
 *   - 2 if either User pointer is NULL
 */
int make_post(const User *author, User *target, char *contents) {
    User *a = (User *) author;
    User *t = target;
     // check that both users exist
    if(a == NULL || t == NULL){
        return 2;
    }
// CHECK THAT THEY ARE FRIENDS AND RETURN 1 IF NOT
    int are_friends = check_if_friends(a, t);
    if(are_friends == 0){
        return 1;
    }
    // now that it has been verified that they both exist and are friends
    // create the new post
    time_t *post_time = malloc(sizeof(time_t));
    *post_time = time(post_time);
    Post *new_post = malloc(sizeof(Post));
    strcpy(new_post->author, a->name);
    new_post->contents = contents;
    new_post->date = post_time;
    new_post->next = NULL;

    // add it in
    // if the target user does not have any posts just make it the first post
    if(target->first_post == NULL){
        target->first_post = new_post;
    }
    else {
        // otherwise
        // store a pointer to the rest of the linked list
        Post *rest_of_posts = target->first_post;
        // make the target's first post point to the memory address of the new_post
        target->first_post = new_post;
        // make new_post next point to the rest of the linked list of posts
        new_post->next = rest_of_posts;
        // return 0 upon successfully making the post
    }
    return 0;
}



/*
 * From the list pointed to by *user_ptr_del, delete the user
 * with the given name.
 * Remove the deleted user from any lists of friends.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user with this name does not exist.
 */
int delete_user(const char *name, User **user_ptr_del) {
    // find the user with that username
    User *delete_me = find_user(name, *user_ptr_del);
    // if it does not exist, return 1
    if (delete_me == NULL){
        return 1;
    }

    if(delete_me->first_post != NULL){
        Post *curr_post = delete_me->first_post;
        while(curr_post != NULL){
            Post *temp = curr_post->next;
            free(curr_post->date);
            free(curr_post->contents);
            free(curr_post);
            curr_post = temp;
        }
        delete_me->first_post = NULL;
    }

    // go through the friends of the user we are deleting
    for(int index = 0; index < MAX_FRIENDS; index++){
        // go to every spot in it
        User *friend_of_deleted = delete_me->friends[index];
        // if there's a friend in that spot, gotta go delete delete_me from its friends array
        if(friend_of_deleted != NULL){
            // go through its list of friends and find delete_me, then change it to null
            for(int j = 0; j < MAX_FRIENDS; j++){
                if(friend_of_deleted->friends[j] == delete_me){
                    friend_of_deleted->friends[j] = NULL;
                    break;
                }
            }
            // set delete_me->friends at that index to be NULL
            delete_me->friends[index] = NULL;
        }

    }

    // go through the linked list of users and delete the user u want to delete

    // special case: delete_me is at the beginning of the linked list
    if(*user_ptr_del == delete_me){
        *user_ptr_del = delete_me->next;
        free(delete_me);
        return 0;
    }
    // start at the beginning of the Linked List
    User *curr_user = *user_ptr_del;
    // find the node aka the user you want to delete
    // dont need a stopping condition for if we reach the end of the linked list
    // we determined earlier in the code that delete_me IS in the linked list.
    // would have already returned 1 if that were not the case,
    // in other words it is CERTAIN that we'll find the delete_me before reaching the end
    while(curr_user->next != delete_me){
        curr_user = curr_user->next;
    }
    // once delete_me has been found, store the link to the node (user) after it
    User *after_deleted = curr_user->next->next;
    // make the node before the one being deleted point to the user after the one being deleted
    curr_user->next = after_deleted;
    // free memory for the node (user) that's being deleted aka delete_me
    free(delete_me);
    return 0;
}
