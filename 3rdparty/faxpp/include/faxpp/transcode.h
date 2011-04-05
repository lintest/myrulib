/*
 * Copyright 2007 Doxological Ltd.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FAXPP__TRANSCODE_H
#define __FAXPP__TRANSCODE_H

#ifdef _MSC_VER
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

/// A single unicode codepoint
typedef uint32_t Char32;

#define TRANSCODE_ERROR 1000
#define TRANSCODE_PREMATURE_END_OF_BUFFER 1001
#define TRANSCODE_BAD_ENCODING 1002

/**
 * Decodes a single character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_utf8_decode(), FAXPP_utf16_le_decode(), FAXPP_utf16_be_decode(), FAXPP_utf16_native_decode(),
 * FAXPP_ucs4_le_decode(), FAXPP_ucs4_be_decode(), FAXPP_ucs4_native_decode()
 */
typedef unsigned int (*FAXPP_DecodeFunction)
     (const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Encodes a single unicode codepoint into the given buffer.
 *
 * \param buffer The buffer to encode to
 * \param buffer_end A pointer to the end of the buffer
 * \param ch The unicode codepoint to encode
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If the buffer does not have enough space
 * to encode the unicode codepoint
 *
 * \see FAXPP_utf8_encode(), FAXPP_utf16_native_encode()
 */
typedef unsigned int (*FAXPP_EncodeFunction)
     (void *buffer, void *buffer_end, Char32 ch);

/**
 * Encapsulates the information needed to both decode and encode
 * an encoding.
 *
 * \see FAXPP_utf8_transcoder, FAXPP_utf16_native_transcoder
 */
typedef struct {
  FAXPP_DecodeFunction decode; ///< The decode function
  FAXPP_EncodeFunction encode; ///< The encode function
} FAXPP_Transcoder;

/**
 * Transcoder to and from UTF-8
 *
 * \see FAXPP_utf8_decode, FAXPP_utf8_encode
 */
extern const FAXPP_Transcoder FAXPP_utf8_transcoder;
/**
 * Transcoder to and from native endian UTF-16
 *
 * \see FAXPP_utf16_native_decode, FAXPP_utf16_native_encode
 */
extern const FAXPP_Transcoder FAXPP_utf16_native_transcoder;

/**
 * Returns a string describing the given (built-in) decode function
 * \param t
 * \return a string
 */
const char *FAXPP_decode_to_string(FAXPP_DecodeFunction t);

/**
 * Returns a string describing the given (built-in) encode function
 * \param t
 * \return a string
 */
const char *FAXPP_encode_to_string(FAXPP_EncodeFunction t);

/**
 * Returns a FAXPP_DecodeFunction for the given encoding string, or 0
 * if one is not supported internally.
 * \param encodingName The encoding to look up
 * \return a FAXPP_DecodeFunction
 */
FAXPP_DecodeFunction FAXPP_string_to_decode(const char *encodingName);

/**
 * Returns a FAXPP_EncodeFunction for the given encoding string, or 0
 * if one is not supported internally.
 * \param encodingName The encoding to look up
 * \return a FAXPP_EncodeFunction
 */
FAXPP_EncodeFunction FAXPP_string_to_encode(const char *encodingName);

/**
 * Decodes a single UTF-8 character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_utf8_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single ISO-8859-1 (Latin1) character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_iso_8859_1_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UTF-16 little endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_utf16_le_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UTF-16 big endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_utf16_be_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UTF-16 native endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_utf16_native_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UCS-4 little endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_ucs4_le_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UCS-4 big endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_ucs4_be_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Decodes a single UCS-4 native endian character from the buffer into it's unicode codepoint.
 *
 * \param buffer The buffer to decode from
 * \param buffer_end A pointer to the end of the buffer
 * \param[out] ch The decoded unicode codepoint
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If there is not enough buffer left to
 * decode another character
 * \retval TRANSCODE_BAD_ENCODING If the the encoding is invalid
 *
 * \see FAXPP_DecodeFunction
 */
unsigned int FAXPP_ucs4_native_decode(const void *buffer, const void *buffer_end, Char32 *ch);

/**
 * Encodes a single unicode codepoint as UTF-8 into the given buffer.
 *
 * \param buffer The buffer to encode to
 * \param buffer_end A pointer to the end of the buffer
 * \param ch The unicode codepoint to encode
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If the buffer does not have enough space
 * to encode the unicode codepoint
 *
 * \see FAXPP_EncodeFunction
 */
unsigned int FAXPP_utf8_encode(void *buffer, void *buffer_end, Char32 ch);

/**
 * Encodes a single unicode codepoint as UTF-16 (native endian) into the given buffer.
 *
 * \param buffer The buffer to encode to
 * \param buffer_end A pointer to the end of the buffer
 * \param ch The unicode codepoint to encode
 *
 * \return The length of the char, unless it is bigger than TRANSCODE_ERROR,
 * in which case it is an error code.
 *
 * \retval TRANSCODE_PREMATURE_END_OF_BUFFER If the buffer does not have enough space
 * to encode the unicode codepoint
 *
 * \see FAXPP_EncodeFunction
 */
unsigned int FAXPP_utf16_native_encode(void *buffer, void *buffer_end, Char32 ch);

#endif
