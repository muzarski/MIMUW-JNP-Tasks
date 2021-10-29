#ifndef MAPTEL_H
#define MAPTEL_H

#ifdef __cplusplus
#include <cstddef>
#include <iostream>
extern "C" {
    namespace jnp1 {
#else
#include <stddef.h>
#include <stdio.h>
#endif

        // Maximal length of the number.
        const size_t TEL_NUM_MAX_LEN = 22;

        // Creates a dictionary and returns its id.
        unsigned long maptel_create(void);

        // Deletes a dictionary with the corresponding id.
        void maptel_delete(unsigned long id);

        // Inserts the information about the change of the number (tel_src -> tel_dst)
        // to the dictionary with the corresponding id. Overwrites any existing information.
        void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst);

        // Removes the information about the change of the tel_src number only if the
        // dictionary with the corresponding id contains it. Otherwise, leaves everything as is.
        void maptel_erase(unsigned long id, char const *tel_src);

        // Checks whether a dictionary with the corresponding id contains the information about
        // changing tel_src. Follows the path of the consecutive changes. Saves the changed number in tel_dst.
        // If there is no change to the number, or the changes form a cycle, it saves number of tel_src in tel_dst.
        // Value len is the size of the memory that tel_dst points to.
        void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len);

#ifdef __cplusplus
    }
};
#endif
#endif // MAPTEL_H
