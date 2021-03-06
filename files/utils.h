/*
 *  utils.h - Common utility routines.
 *
 *  Copyright (C) 1998-1999  Jeffrey S. Freedman
 *  Copyright (C) 2000-2013  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <stdio.h>
#include <iosfwd>
#include <dirent.h>

#include "common_types.h"
#include "rect.h"

#ifndef ATTR_PRINTF
#ifdef __GNUC__
#define ATTR_PRINTF(x,y) __attribute__((format(printf, (x), (y))))
#else
#define ATTR_PRINTF(x,y)
#endif
#endif

#ifndef HAVE_SNPRINTF
extern int snprintf(char *, size_t, const char *, /*args*/ ...) ATTR_PRINTF(3,4);
#endif

/*
 *  Read a 1-byte value.
 */

inline uint8 Read1(
    std::istream& in
) {
	return static_cast<uint8>(in.get());
}

inline uint8 Read1(
    std::istream *in
) {
	return static_cast<uint8>(in->get());
}

inline uint8 Read1(
    const uint8 *& in
) {
	return *in++;
}

inline uint8 Read1(
    std::FILE *in
) {
	uint8 b0;
	if (std::fread(&b0, sizeof(uint8), 1, in) != 1)
		b0 = 0;
	return b0;
}

/*
 *  Read a 2-byte value, lsb first.
 */

template <typename Source>
inline uint16 Read2(
    Source& in
) {
	uint16 b0 = Read1(in);
	uint16 b1 = Read1(in);
	return (b1 << 8) | b0;
}

/*
 *  Read a 2-byte value, hsb first.
 */

template <typename Source>
inline uint16 Read2high(
    Source& in
) {
	uint16 b0 = Read1(in);
	uint16 b1 = Read1(in);
	return (b0 << 8) | b1;
}

/*
 *  Read a 4-byte long value, lsb first.
 */

template <typename Source>
inline uint32 Read4(
    Source& in
) {
	uint32 b0 = Read1(in);
	uint32 b1 = Read1(in);
	uint32 b2 = Read1(in);
	uint32 b3 = Read1(in);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

/*
 *  Read a 4-byte long value, hsb first.
 */
template <typename Source>
inline uint32 Read4high(
    Source& in
) {
	uint32 b0 = Read1(in);
	uint32 b1 = Read1(in);
	uint32 b2 = Read1(in);
	uint32 b3 = Read1(in);
	return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

/*
 *  Read a 2-byte value, lsb first, unsigned.
 */
template <typename Source>
inline sint16 Read2s(
    Source& in
) {
	return static_cast<sint16>(Read2(in));
}

/*
 *  Read a 4-byte value, lsb first, unsigned.
 */
template <typename Source>
inline sint32 Read4s(
    Source& in
) {
	return static_cast<sint32>(Read4(in));
}

/*
 *  Read a N-byte long value, lsb first.
 */

template<typename T, typename Source>
inline T ReadN(
    Source& in
) {
	T val = 0;
	for (size_t i = 0; i < sizeof(T); i++)
		val |= static_cast<T>(T(Read1(in)) << (8 * i));
	return val;
}

/*
 *  Read a N-byte long value, hsb first.
 */

template<typename T, typename Source>
inline T ReadNhigh(
    Source& in
) {
	T val = 0;
	for (int i = sizeof(T) - 1; i >= 0; i--)
		val |= static_cast<T>(T(Read1(in)) << (8 * i));
	return val;
}

inline int ReadInt(
    std::istream &in,
    int def = 0
) {
	int num;
	if (in.eof())
		return def;
	in >> num;
	if (in.fail())
		return def;
	in.ignore(0xffffff, '/');
	return num;
}

inline unsigned int ReadUInt(
    std::istream &in,
    int def = 0
) {
	unsigned int num;
	if (in.eof())
		return def;
	in >> num;
	if (in.fail())
		return def;
	in.ignore(0xffffff, '/');
	return num;
}

inline void WriteInt(
    std::ostream &out,
    int num,
    bool final = false
) {
	out << num;
	if (final)
		out << std::endl;
	else
		out << '/';
}

inline void WriteInt(
    std::ostream &out,
    unsigned int num,
    bool final = false
) {
	out << num;
	if (final)
		out << std::endl;
	else
		out << '/';
}

inline void ReadRect(
    std::istream &in,
    Rectangle &rect
) {
	rect.x = ReadInt(in);
	rect.y = ReadInt(in);
	rect.w = ReadInt(in);
	rect.h = ReadInt(in);
}

inline void WriteRect(
    std::ostream &out,
    const Rectangle &rect,
    bool final = false
) {
	WriteInt(out, rect.x);
	WriteInt(out, rect.y);
	WriteInt(out, rect.w);
	WriteInt(out, rect.h, final);
}

inline std::string ReadStr(
    char *&eptr,
    int off = 1
) {
	eptr += off;
	char *pos = std::strchr(eptr, '/');
	std::string retval(eptr, pos - eptr);
	eptr = pos;
	return retval;
}

inline std::string ReadStr(
    std::istream &in
) {
	std::string retval;
	std::getline(in, retval, '/');
	return retval;
}

inline void WriteStr(
    std::ostream &out,
    const std::string &str,
    bool final = false
) {
	out << str;
	if (final)
		out << std::endl;
	else
		out << '/';
}

/*
 *  Read a 1-byte value from mutable data.
 */

inline uint8 MRead1(
    uint8 *& in
) {
	return *in++;
}

/*
 *  Read a 2-byte value, lsb first, from mutable data.
 */

inline uint16 MRead2(
    uint8 *& in
) {
	uint16 b0 = MRead1(in);
	uint16 b1 = MRead1(in);
	return (b1 << 8) | b0;
}

/*
 *  Write a 1-byte value.
 */

inline void Write1(
    std::ostream &out,
    uint8 val
) {
	out.put(static_cast<char>(val));
}

inline void Write1(
    std::ostream *out,
    uint8 val
) {
	out->put(static_cast<char>(val));
}

inline void Write1(
    uint8 *& out,
    uint8 val
) {
	*out++ = val;
}

inline void Write1(
    std::FILE *out,
    uint8 val
) {
	std::fputc(val, out);
}

/*
 *  Write a 2-byte value, lsb first.
 */

template <typename Dest>
inline void Write2(
    Dest& out,
    uint16 val
) {
	Write1(out, val & 0xffu);
	Write1(out, (val >> 8) & 0xffu);
}

/*
 *  Write a 2-byte value, msb first.
 */

template <typename Dest>
inline void Write2high(
    Dest& out,
    uint16 val
) {
	Write1(out, (val >> 8) & 0xffu);
	Write1(out, val & 0xffu);
}


/*
 *  Write a 4-byte value, lsb first.
 */

template <typename Dest>
inline void Write4(
    Dest& out,
    uint32 val
) {
	Write1(out, val & 0xffu);
	Write1(out, (val >> 8) & 0xffu);
	Write1(out, (val >> 16) & 0xffu);
	Write1(out, (val >> 24) & 0xffu);
}

/*
 *  Write a 4-byte value, msb first.
 */

template <typename Dest>
inline void Write4high(
    Dest& out,
    uint32 val
) {
	Write1(out, (val >> 24) & 0xffu);
	Write1(out, (val >> 16) & 0xffu);
	Write1(out, (val >> 8) & 0xffu);
	Write1(out, val & 0xffu);
}

/*
 *  Write a signed 4-byte value to a stream, lsb first.
 */

template <typename Dest>
inline void Write4s(
    Dest& out,
    sint32 val
) {
	Write4(out, static_cast<uint32>(val));
}

/*
 *  Write a N-byte value, lsb first.
 */

template<typename T, typename Dest>
inline void WriteN(
    Dest& out,
    T val
) {
	for (size_t i = 0; i < sizeof(T); i++)
		Write1(out, (val >>(8 * i)) & 0xffu);
}

/*
 *  Write a N-byte value, msb first.
 */

template<typename T, typename Dest>
inline void WriteNhigh(
    Dest& out,
    T val
) {
	for (int i = sizeof(T) - 1; i >= 0 ; i--)
		Write1(out, (val >>(8 * i)) & 0xffu);
}

bool U7open(
    std::ifstream &in,          // Input stream to open.
    const char *fname,          // May be converted to upper-case.
    bool is_text = false            // Should the file be opened in text mode
);
bool U7open(
    std::ofstream &out,         // Output stream to open.
    const char *fname,          // May be converted to upper-case.
    bool is_text = false            // Should the file be opened in text mode
);

std::FILE *U7open(
    const char *fname,          // May be converted to upper-case.
    const char *mode            // File access mode.
);

bool U7open_static(
    std::ifstream &in,      // Input stream to open.
    const char *fname,      // May be converted to upper-case.
    bool is_text            // Should file be opened in text mode
);
DIR *U7opendir(
    const char *fname			// May be converted to upper-case.
);
void U7remove(
    const char *fname
);

bool U7exists(
    const char *fname
);

inline bool U7exists(std::string fname) {
	return U7exists(fname.c_str());
}

int U7mkdir(
    const char *dirname,
    int mode
);

#ifdef WIN32
void redirect_output(const char *prefix = "std");
void cleanup_output(const char *prefix = "std");
#endif
void setup_data_dir(const std::string &data_path, const char *runpath);
void setup_program_paths();

int U7chdir(
    const char *dirname
);

void U7copy(
    const char *src,
    const char *dest
);

bool is_system_path_defined(const std::string &key);
void store_system_paths();
void reset_system_paths();
void clear_system_path(const std::string &key);
void add_system_path(const std::string &key, const std::string &value);
void clone_system_path(const std::string &new_key, const std::string &old_key);
std::string get_system_path(const std::string &path);

#ifdef MACOSX
#define BUNDLE_CHECK(x,y) ((is_system_path_defined("<BUNDLE>") && U7exists((x)))  ? (x) : (y))
#else
#define BUNDLE_CHECK(x,y) (y)
#endif

void to_uppercase(std::string &str);
std::string to_uppercase(const std::string &str);

int Log2(uint32 n);
uint32 msb32(uint32 n);
int fgepow2(uint32 n);

char *newstrdup(const char *s);
char *Get_mapped_name(const char *from, int num, char *to);
int Find_next_map(int start, int maxtry);


inline int bitcount(unsigned char n) {
#define TWO(c)     (0x1u << (c))
#define MASK(c)    ((static_cast<uint32>(-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x,c) ((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c))
	// Only works for 8-bit numbers.
	n = static_cast<unsigned char>(COUNT(n, 0));
	n = static_cast<unsigned char>(COUNT(n, 1));
	n = static_cast<unsigned char>(COUNT(n, 2));
	return n;
}

#endif  /* _UTILS_H_ */
