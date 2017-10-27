#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

/**
 * @brief Wrong syntax configuration file exception
 *
 * Exception for wrong syntax in configuration file.
 */
struct wrong_syntax_configuration_file_exception : std::exception {
    explicit wrong_syntax_configuration_file_exception() noexcept {}
    virtual ~wrong_syntax_configuration_file_exception() noexcept {}

    virtual const char * what() const noexcept override {
        return "in config file: please make sure that the syntaxes match.\n";
    }
};

/**
 * @brief Wrong grid path
 *
 * Exception for wrong grid path in configuration file.
 */
struct wrong_grid_path : std::exception {
    explicit wrong_grid_path() noexcept {}
    virtual ~wrong_grid_path() noexcept {}

    virtual const char * what() const noexcept override {
        return "in config file: wring grid path.\n";
    }
};

#endif // EXCEPTIONS_HPP_
