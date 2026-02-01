/**
 * @file dynamic_dispatch.h
 * @brief Solution for milestone 02, refactor part.
 */


#ifndef DYNAMIC_DISPATCH_H
#define DYNAMIC_DISPATCH_H


struct payload_buffer {
	struct payload *payloads;
	int len;
	int cap;
	int process_base;
};

struct payload_buffer *new_buffer();

/**
 * @brief Parses and adds a payload to the buffer.
 *
 * Parses the raw payload string, determines its type, assigns appropriate
 * function pointers (process and destroy), and appends it to the buffer. It
 * acts as a constructor, which we see on upcoming milesones.
 *
 * @param buf Pointer to the payload buffer
 * @param raw Raw payload string to parse
 *
 * @note This function assigns the correct process() and destroy() function
 *       pointers based on the payload type, eliminating the need for switch
 *       statements during processing.
 */
void push_payload(struct payload_buffer *buf, const char *raw);

/**
 * @brief Processes the next payload from the buffer using polymorphism.
 *
 * Retrieves the payload at index process_base and calls its process()
 * function pointer. This demonstrates dynamic dispatch, the actual function
 * that runs is determined at runtime based on which function was assigned
 * during push_payload().
 *
 * @param buf Pointer to the payload buffer
 *
 * @note Compare this to Exercise 01: no switch statement needed! The payload
 *       knows how to process itself.
 */
void process_next(struct payload_buffer *buf);

/**
 * @brief Destroys the buffer and all contained payloads.
 *
 * Calls each payload's destroy() function pointer to free type-specific
 * memory, then frees the buffer itself.
 *
 * @param buf Pointer to the payload buffer to destroy
 *
 * @note Again, no switch statement, each payload's destroy() function handles
 *       its own cleanup logic.
 */
void destroy(struct payload_buffer *buf);


#endif
