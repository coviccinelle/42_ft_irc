#ifndef UTILS_HPP
#define UTILS_HPP

typedef std::string 						string;

typedef std::vector< string >				vec_str;
typedef const std::vector< string >			cst_vec_str;

typedef std::vector< vec_str >				vec_vec_str;
typedef const std::vector< vec_str >		cst_vec_vec_str;

typedef std::vector< struct pollfd >		vec_pfd;
typedef const std::vector< struct pollfd >	cst_vec_pfd;

#endif
