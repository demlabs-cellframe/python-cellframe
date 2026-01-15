/**
 * @file cf_callbacks_registry.c
 * @brief Global callback registry implementation
 * @details Provides centralized Python callback lifecycle management to prevent memory leaks.
 * @author Cellframe Development Team
 * @date 2025-01-11
 */

#include "cf_callbacks_registry.h"
#include "dap_common.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "python_cellframe_registry"

// Global registry instance
static cf_callbacks_registry_t s_registry = {
    .head = NULL,
    .count = 0
};

static bool s_registry_initialized = false;

/**
 * @brief Initialize the global callback registry
 */
int cf_callbacks_registry_init(void)
{
    if (s_registry_initialized) {
        log_it(L_WARNING, "Callback registry already initialized");
        return 0;
    }

    pthread_mutex_init(&s_registry.lock, NULL);
    s_registry.head = NULL;
    s_registry.count = 0;
    s_registry_initialized = true;

    log_it(L_INFO, "Python callbacks registry initialized");
    return 0;
}

/**
 * @brief Register a Python callback
 */
int cf_callbacks_registry_add(cf_callback_type_t type, PyObject *py_callback, PyObject *py_arg,
                               void *sdk_context, const char *identifier)
{
    if (!s_registry_initialized) {
        log_it(L_ERROR, "Callback registry not initialized");
        return -1;
    }

    if (!py_callback || !PyCallable_Check(py_callback)) {
        log_it(L_ERROR, "Invalid Python callback object");
        return -2;
    }

    // Allocate new entry
    cf_callback_entry_t *entry = DAP_NEW_Z(cf_callback_entry_t);
    if (!entry) {
        log_it(L_CRITICAL, "Failed to allocate callback entry");
        return -3;
    }

    entry->type = type;
    entry->py_callback = py_callback;
    entry->py_arg = py_arg;
    entry->sdk_context = sdk_context;
    entry->identifier = identifier ? strdup(identifier) : NULL;

    // Increment ref counts for Python objects
    Py_INCREF(py_callback);
    if (py_arg) {
        Py_INCREF(py_arg);
    }

    // Add to linked list (thread-safe)
    pthread_mutex_lock(&s_registry.lock);
    entry->next = s_registry.head;
    s_registry.head = entry;
    s_registry.count++;
    pthread_mutex_unlock(&s_registry.lock);

    log_it(L_DEBUG, "Registered Python callback type=%d, total=%zu", type, s_registry.count);
    return 0;
}

/**
 * @brief Remove and cleanup a callback entry (internal, assumes lock held)
 */
static void s_callback_entry_free(cf_callback_entry_t *entry)
{
    if (!entry)
        return;

    // Decrement Python ref counts (CRITICAL for memory leak prevention)
    if (entry->py_callback) {
        Py_DECREF(entry->py_callback);
    }
    if (entry->py_arg) {
        Py_DECREF(entry->py_arg);
    }

    // Free identifier string
    if (entry->identifier) {
        DAP_DELETE(entry->identifier);
    }

    DAP_DELETE(entry);
}

/**
 * @brief Unregister a specific callback by SDK context
 */
int cf_callbacks_registry_remove(void *sdk_context)
{
    if (!s_registry_initialized)
        return -1;

    pthread_mutex_lock(&s_registry.lock);

    cf_callback_entry_t *prev = NULL;
    cf_callback_entry_t *curr = s_registry.head;

    while (curr) {
        if (curr->sdk_context == sdk_context) {
            // Remove from list
            if (prev) {
                prev->next = curr->next;
            } else {
                s_registry.head = curr->next;
            }

            s_registry.count--;
            pthread_mutex_unlock(&s_registry.lock);

            log_it(L_DEBUG, "Removed Python callback, remaining=%zu", s_registry.count);
            s_callback_entry_free(curr);
            return 0;
        }

        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&s_registry.lock);
    log_it(L_WARNING, "Callback with sdk_context=%p not found", sdk_context);
    return -2;
}

/**
 * @brief Remove all callbacks of a specific type
 */
int cf_callbacks_registry_remove_by_type(cf_callback_type_t type)
{
    if (!s_registry_initialized)
        return 0;

    int removed = 0;
    pthread_mutex_lock(&s_registry.lock);

    cf_callback_entry_t *prev = NULL;
    cf_callback_entry_t *curr = s_registry.head;

    while (curr) {
        cf_callback_entry_t *next = curr->next;

        if (curr->type == type) {
            // Remove from list
            if (prev) {
                prev->next = next;
            } else {
                s_registry.head = next;
            }

            s_registry.count--;
            s_callback_entry_free(curr);
            removed++;
        } else {
            prev = curr;
        }

        curr = next;
    }

    pthread_mutex_unlock(&s_registry.lock);

    if (removed > 0) {
        log_it(L_INFO, "Removed %d Python callbacks of type=%d", removed, type);
    }

    return removed;
}

/**
 * @brief Remove all callbacks for a specific identifier
 */
int cf_callbacks_registry_remove_by_id(const char *identifier)
{
    if (!s_registry_initialized || !identifier)
        return 0;

    int removed = 0;
    pthread_mutex_lock(&s_registry.lock);

    cf_callback_entry_t *prev = NULL;
    cf_callback_entry_t *curr = s_registry.head;

    while (curr) {
        cf_callback_entry_t *next = curr->next;

        if (curr->identifier && strcmp(curr->identifier, identifier) == 0) {
            // Remove from list
            if (prev) {
                prev->next = next;
            } else {
                s_registry.head = next;
            }

            s_registry.count--;
            s_callback_entry_free(curr);
            removed++;
        } else {
            prev = curr;
        }

        curr = next;
    }

    pthread_mutex_unlock(&s_registry.lock);

    if (removed > 0) {
        log_it(L_INFO, "Removed %d Python callbacks for id='%s'", removed, identifier);
    }

    return removed;
}

/**
 * @brief Cleanup all registered callbacks
 */
void cf_callbacks_registry_cleanup_all(void)
{
    if (!s_registry_initialized)
        return;

    pthread_mutex_lock(&s_registry.lock);

    cf_callback_entry_t *curr = s_registry.head;
    size_t cleaned = 0;

    while (curr) {
        cf_callback_entry_t *next = curr->next;
        s_callback_entry_free(curr);
        cleaned++;
        curr = next;
    }

    s_registry.head = NULL;
    s_registry.count = 0;

    pthread_mutex_unlock(&s_registry.lock);

    log_it(L_INFO, "Python callbacks registry cleaned up: %zu callbacks freed", cleaned);
}

/**
 * @brief Deinitialize the callback registry
 */
void cf_callbacks_registry_deinit(void)
{
    if (!s_registry_initialized)
        return;

    cf_callbacks_registry_cleanup_all();
    pthread_mutex_destroy(&s_registry.lock);
    s_registry_initialized = false;

    log_it(L_INFO, "Python callbacks registry deinitialized");
}

/**
 * @brief Get registry statistics
 */
void cf_callbacks_registry_stats(size_t *out_count, size_t *out_by_type)
{
    if (!s_registry_initialized || !out_count)
        return;

    pthread_mutex_lock(&s_registry.lock);

    *out_count = s_registry.count;

    // Count by type if requested
    if (out_by_type) {
        // Initialize all counts to 0
        for (int i = 0; i < CF_CALLBACK_TYPE_LAST; i++) {
            out_by_type[i] = 0;
        }

        // Count each type
        cf_callback_entry_t *curr = s_registry.head;
        while (curr) {
            if (curr->type >= 0 && curr->type < CF_CALLBACK_TYPE_LAST) {
                out_by_type[curr->type]++;
            }
            curr = curr->next;
        }
    }

    pthread_mutex_unlock(&s_registry.lock);
}
