#ifndef __emscripten_fetch_h__
#define __emscripten_fetch_h__

#include <limits.h>
#include <inttypes.h>
#include <emscripten/html5.h>

#ifdef __cplusplus
extern "C" {
#endif

struct emscripten_fetch_t
{
	// Unique identifier for this fetch in progress.
	unsigned int id;

	// Custom data that can be tagged along the process.
	void *userData;

	// The remote URL that is being downloaded.
	const char *url;

	// In onsuccess() handler:
	//   - If the EMSCRIPTEN_FETCH_LOAD_TO_MEMORY attribute was specified for the transfer, this points to the
	//     body of the downloaded data. Otherwise this will be null.
	// In onprogress() handler:
	//   - If the EMSCRIPTEN_FETCH_STREAM_DATA attribute was specified for the transfer, this points to a partial
	//     chunk of bytes related to the transfer. Otherwise this will be null.
	const char *data;

	// Specifies the length of the above data block in bytes. When the download finishes, this field will be valid even if
	// EMSCRIPTEN_FETCH_LOAD_TO_MEMORY was not specified.
	uint64_t numBytes;

	// If EMSCRIPTEN_FETCH_STREAM_DATA is being performed, this indicates the byte offset from the start of the stream
	// that the data block specifies. (for onprogress() streaming XHR transfer, the number of bytes downloaded so far before this chunk)
	uint64_t dataOffset;

	// Specifies the total number of bytes that the response body will be.
	uint64_t totalBytes;
};

// Emscripten fetch attributes:
// If passed, the body of the request will be present in full in the onsuccess() handler.
#define EMSCRIPTEN_FETCH_LOAD_TO_MEMORY  1

// If passed, the intermediate streamed bytes will be passed in to the onprogress() handler. If not specified, the
// onprogress() handler will still be called, but without data bytes.
#define EMSCRIPTEN_FETCH_STREAM_DATA 2

// If passed, the final download will be stored in IndexedDB. If not specified, the file will only reside in browser memory.
#define EMSCRIPTEN_FETCH_PERSIST_FILE 4

// If the file already exists in IndexedDB, it is returned without redownload. If a partial transfer exists in IndexedDB,
// the download will resume from where it left off and run to completion.
// EMSCRIPTEN_FETCH_APPEND, EMSCRIPTEN_FETCH_REPLACE and EMSCRIPTEN_FETCH_NO_DOWNLOAD are mutually exclusive.
#define EMSCRIPTEN_FETCH_APPEND 8

// If the file already exists in IndexedDB, the old file will be deleted and a new download is started.
// EMSCRIPTEN_FETCH_APPEND, EMSCRIPTEN_FETCH_REPLACE and EMSCRIPTEN_FETCH_NO_DOWNLOAD are mutually exclusive.
#define EMSCRIPTEN_FETCH_REPLACE 16

// If specified, the file will only be looked up in IndexedDB, but if it does not exist, it is not attempted to be downloaded
// over the network but an error is raised.
// EMSCRIPTEN_FETCH_APPEND, EMSCRIPTEN_FETCH_REPLACE and EMSCRIPTEN_FETCH_NO_DOWNLOAD are mutually exclusive.
#define EMSCRIPTEN_FETCH_NO_DOWNLOAD 32

// Specifies the parameters for a newly initiated fetch operation.
struct emscripten_fetch_attr_t
{
	// 'POST', 'GET', etc.
	char requestType[32];

	// Custom data that can be tagged along the process.
	void *userData;

	void (*onsuccess)(emscripten_fetch_t *fetch);
	void (*onerror)(emscripten_fetch_t *fetch);
	void (*onprogress)(emscripten_fetch_t *fetch);

	// Specifies the destination path in IndexedDB where to store the downloaded content body. If this is empty, the transfer
	// is not stored to IndexedDB at all.
	char destinationPath[PATH_MAX];

	// EMSCRIPTEN_FETCH_* attributes
	uint32_t attributes;
};

// Clears the fields of an emscripten_fetch_attr_t structure to their default values in a future-compatible manner.
void emscripten_fetch_attr_init(emscripten_fetch_attr_t *fetch_attr);

// Initiates a new Emscripten fetch operation, which downloads data from the given URL or from IndexedDB database.
emscripten_fetch_t *emscripten_fetch(emscripten_fetch_attr_t *fetch_attr, const char *url);

// Synchronously blocks to wait for the given fetch operation to complete. This operation is not allowed in the main browser
// thread, in which case it will return EMSCRIPTEN_RESULT_NOT_SUPPORTED. Pass timeoutMSecs=infinite to wait indefinitely. If
// the wait times out, the return value will be EMSCRIPTEN_RESULT_TIMEOUT.
// The onsuccess()/onerror()/onprogress() handlers will be called in the calling thread from within this function before
// this function returns.
EMSCRIPTEN_RESULT emscripten_fetch_wait(emscripten_fetch_t *fetch, double timeoutMsecs);

// Closes a finished or an executing fetch operation and frees up all memory. If the fetch operation was still executing, the
// onerror() handler will be called in the calling thread before this function returns.
EMSCRIPTEN_RESULT emscripten_fetch_close(emscripten_fetch_t *fetch);

#ifdef __cplusplus
}
#endif

// ~__emscripten_fetch_h__
#endif
