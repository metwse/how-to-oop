/**
 * @file payload.h
 * @brief Refactored payload system using function pointers for polymorphic
 *        behavior.
 *
 * This header demonstrates the function pointer approach to polymorphism.
 * Instead of using enums and switch statements, each payload stores pointers
 * to its own processing and cleanup functions, enabling dynamic dispatch
 * without type tags.
 *
 * Changes over Exercise 01:
 * - No payload_kind enum
 * - Each payload is self-contained with its own behavior
 */


#ifndef PAYLOAD_H
#define PAYLOAD_H


/**
 * @brief Union containing type-specific data for each payload kind.
 *
 * Same structure as Exercise 01, but now used with function pointers instead
 * of enum-based dispatching.
 */
union payload_data {
	struct {
		char *username;
		char *password;
	} command_login;

	struct {
		char *channel;
	} command_join;

	struct {
		char *username;
		char *content;
	} message_direct;

	struct {
		char *channel;
		char *content;
	} message_group;

	struct {
		char *content;
	} message_global;
};

/**
 * @brief Represents a single parsed payload (function pointer pattern).
 *
 * This structure demonstrates polymorphism through function pointers.
 * Instead of checking a type tag, each payload knows how to process
 * and destroy itself through stored function pointers.
 *
 * @note The function pointers are assigned during payload creation based
 *       on the payload type parsed from the input string.
 */
struct payload {
	void (*process)(const struct payload *self);   /**< Payload behaivor */
	void (*destroy)(const struct payload *self);   /**< Cleanup strategy */
	union payload_data data;
};

/* process strategies */
void process_command_login(const struct payload *self);
void process_command_join(const struct payload *self);
void process_command_logout(const struct payload *self);
void process_message_direct(const struct payload *self);
void process_message_group(const struct payload *self);
void process_message_global(const struct payload *self);

/* cleanup strategies */
void destroy_command_login(const struct payload *self);
void destroy_command_join(const struct payload *self);
void destroy_command_logout(const struct payload *self);
void destroy_message_direct(const struct payload *self);
void destroy_message_group(const struct payload *self);
void destroy_message_global(const struct payload *self);


#endif
