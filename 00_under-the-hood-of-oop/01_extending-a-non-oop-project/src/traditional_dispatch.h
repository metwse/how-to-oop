/**
 * @file traditional_dispatch.h
 * @brief Solution for milestone 01
 *
 * This header demonstrates the traditional C approach to polymorphism using
 * tagged unions (discriminated unions). Each payload type is explicitly
 * enumerated and handled through switch/if-else logic, showcasing the
 * limitations of non-OOP patterns when extending functionality.
 */


#ifndef TRADITIONAL_DISPATCH_H
#define TRADITIONAL_DISPATCH_H


/**
 * @brief Enumeration of all possible payload types.
 *
 * This enum serves as the "tag" in the tagged union pattern, identifying
 * which member of the payload_data union is valid for a given payload.
 *
 * @note Adding new payload types requires modifying this enum, the union,
 *       and all switch statements that process payloads.
 */
enum payload_kind {
	COMMAND_LOGIN,   /**< Login command with username and password */
	COMMAND_JOIN,    /**< Join channel command */
	COMMAND_LOGOUT,  /**< Logout command (no arguments) */
	MESSAGE_DIRECT,  /**< Direct message to a specific user */
	MESSAGE_GROUP,   /**< Group message to a channel */
	MESSAGE_GLOBAL   /**< Global broadcast message */
};

/**
 * @brief Union containing type-specific data for each payload kind.
 *
 * Each member corresponds to a specific payload_kind and contains the parsed
 * arguments for that payload type. The correct member must be accessed based
 * on the payload's kind field.
 */
union payload_data {
	/** @brief Data for COMMAND_LOGIN payload. */
	struct {
		char *username;  /**< Dynamically allocated username string */
		char *password;  /**< Dynamically allocated password string */
	} command_login;

	/** @brief Data for COMMAND_JOIN payload. */
	struct {
		char *channel;   /**< Dynamically allocated channel name */
	} command_join;

	/* COMMAND_LOGOUT has no associated data */

	/** @brief Data for MESSAGE_DIRECT payload. */
	struct {
		char *username;  /**< Dynamically allocated recipient username */
		char *content;   /**< Dynamically allocated message content */
	} message_direct;

	/** @brief Data for MESSAGE_GROUP payload. */
	struct {
		char *channel;   /**< Dynamically allocated channel name */
		char *content;   /**< Dynamically allocated message content */
	} message_group;

	/** @brief Data for MESSAGE_GLOBAL payload. */
	struct {
		char *content;   /**< Dynamically allocated message content */
	} message_global;
};

/**
 * @brief Represents a single parsed payload (tagged union pattern).
 *
 * Combines a type tag (kind) with type-specific data (data union). The kind
 * field determines which member of the data union is valid.
 */
struct payload {
	enum payload_kind kind;    /**< Type discriminator */
	union payload_data data;   /**< Type-specific payload data */
};

/**
 * @brief Dynamic buffer for storing and processing payloads.
 *
 * Implements a growable array of payloads with separate read and process
 * phases. The buffer automatically grows when capacity is reached.
 */
struct payload_buffer {
	struct payload *payloads;  /**< Dynamic array of payloads */
	int len;                   /**< Current number of payloads in buffer */
	int cap;                   /**< Allocated capacity of payloads array */
	int process_base;          /**< Index of next payload to process */
};

/**
 * @brief Creates a new empty payload buffer.
 *
 * Allocates and initializes a new payload buffer with an initial capacity.
 * The buffer starts empty (len=0) and with process_base=0.
 *
 * @return Pointer to newly allocated payload_buffer
 */
struct payload_buffer *new_buffer();

/**
 * @brief Parses and adds a payload to the buffer.
 *
 * Parses the raw payload string, determines its type, extracts arguments, and
 * appends it to the buffer. Automatically grows the buffer if needed. All
 * string fields in the payload are dynamically allocated.
 *
 * @param buf Pointer to the payload buffer
 * @param payload Raw payload string to parse (e.g., "/login alice pass123")
 *
 * @note The payload string is parsed immediately; the original string is not
 *       stored
 * @note All dynamically allocated strings must be freed when the payload is
 *       destroyed
 */
void push_payload(struct payload_buffer *buf, const char *payload);

/**
 * @brief Processes and prints the next payload from the buffer.
 *
 * Retrieves the payload at index process_base, prints it according to its
 * type, and increments process_base. The format of the output depends on the
 * payload kind.
 *
 * @param buf Pointer to the payload buffer
 *
 * @note This function should only be called when process_base < len
 * @note Does not free the payload, cleanup must be done separately
 */
void process_next(struct payload_buffer *buf);

/**
 * @brief Free all of the resources used by payload buffer.
 *
 * Cleans up the resources used by buffer and payloads in it.
 *
 * @param buf Pointer to the payload buffer
 */
void destroy(struct payload_buffer *buf);


#endif
