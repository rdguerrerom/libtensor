#include <cstdio>
#include <cstring>
#include "exception.h"

namespace libutil {


void exception::init(const char *ns, const char *clazz, const char *method,
    const char *file, unsigned int line, const char *type,
    const char *message) throw() {

    if(ns == NULL) m_ns[0] = '\0';
    else { strncpy(m_ns, ns, 128); m_ns[127] = '\0'; }
    if(clazz == NULL) m_clazz[0] = '\0';
    else { strncpy(m_clazz, clazz, 128); m_clazz[127] = '\0'; }
    if(method == NULL) m_method[0] = '\0';
    else { strncpy(m_method, method, 128); m_method[127] = '\0'; }
    if(file == NULL) m_file[0] = '\0';
    else { strncpy(m_file, file, 128); m_file[127] = '\0'; }
    m_line = line;
    if(type == NULL) m_type[0] = '\0';
    else { strncpy(m_type, type, 128); m_type[127] = '\0'; }
    if(message == NULL) m_message[0] = '\0';
    else { strncpy(m_message, message, 256); m_message[255] = '\0'; }

    if(strlen(m_type) == 0) {
        strcpy(m_type, "exception");
    }
    if(strlen(m_message) == 0) {
        strcpy(m_message, "<No error message>");
    }

    if(strlen(m_ns) == 0) {
    if(strlen(m_clazz) == 0) {
    if(strlen(m_method) == 0) {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s\n%s", m_type, m_message);
    else
        snprintf(m_what, 1024, "%s (%u), %s\n%s",
            m_file, m_line, m_type, m_message);
    } else {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s, %s\n%s",
            m_method, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s, %s (%u), %s\n%s",
            m_method, m_file, m_line, m_type, m_message);
    }
    } else {
    if(strlen(m_method) == 0) {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s, %s\n%s",
            m_clazz, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s, %s (%u), %s\n%s",
            m_clazz, m_file, m_line, m_type, m_message);
    } else {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s::%s, %s\n%s",
            m_clazz, m_method, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s::%s, %s (%u), %s\n%s",
            m_clazz, m_method, m_file, m_line, m_type, m_message);
    }
    }
    } else {
    if(strlen(m_clazz) == 0) {
    if(strlen(m_method) == 0) {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s, %s\n%s", m_ns, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s, %s (%u), %s\n%s",
            m_ns, m_file, m_line, m_type, m_message);
    } else {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s::%s, %s\n%s",
            m_ns, m_method, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s::%s, %s (%u), %s\n%s",
            m_ns, m_method, m_file, m_line, m_type, m_message);
    }
    } else {
    if(strlen(m_method) == 0) {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s::%s, %s\n%s",
            m_ns, m_clazz, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s::%s, %s (%u), %s\n%s",
            m_ns, m_clazz, m_file, m_line, m_type, m_message);
    } else {
    if(strlen(m_file) == 0)
        snprintf(m_what, 1024, "%s::%s::%s, %s\n%s",
            m_ns, m_clazz, m_method, m_type, m_message);
    else
        snprintf(m_what, 1024, "%s::%s::%s, %s (%u), %s\n%s",
            m_ns, m_clazz, m_method, m_file, m_line,
            m_type, m_message);
    }
    }
    }

}


const char *exception::what() const throw() {

    return m_what;
}

} // namespace libutil

