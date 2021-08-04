# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- log\_argp argument `--no-syslog` for daemon logs as there would be no way to
  disable default syslog otherwise.

### Changed
- Syslog configuration is now no longer configured using boolean in the log
  structure but rather by using functions with default handled the same way as
  for standard error fallback output.

### Removed
- log\_argp argument `--no-stderr` as it is easy to redirect or drop standard
  error output without needing this option


## [0.3.0] - 2021-05-22
### Added 
- support for binding logs to allow combination of multiple log instances to
  single one
- signal masking for `_logc` function and thus for all log outputing functions

### Changed
- Some functions return defaults directly without allocating internal log structure

### Fixed
- detection of distribution archive for gperf detection when building outside of
  source files tree
- custom syslog format memory leak


## [0.2.1] - 2021-05-25
### Fixed
- version specification in pkg-config files
- distribute archives
- code coverage generation library inclusion

### Changed
- Missing `cppcheck` is not fatal error if linters are enabled


## [0.2.0] - 2021-05-24
### Added
- Details about compilation and testing to README.adoc
- Output format fragment `LOG_FP_LEVEL_NAME`
- Output format field `%|` that is handled as "else" inside conditions
- Support for output level offset
- Function `log_offset_level` to set level relative to current setting
- Support for output to syslog
- New log creation helper macro `DAEMON_LOG` for applications commonly started in
  background
- `--log-file` argp option
- `--log-level` argp otion

### Changed
- Log macros for specific log prefixed with `log_`
- Log macros using `DEFLOG` renamed from upper-case to lower-case
- Macro `log` was renamed to `logc` to not collide with `math.h`'s `log`
- Function `_log` was renamed to `_logc` to be consistent with `log` rename
- Code is now compiled explicitly with C11 standard support
- Output format not-empty condition do not investigate sub-conditions, instead it
  considers them non-empty if they condition is fulfilled.
- Default formats now use `LOG_FP_LEVEL_NAME` for message type representation
- How message level and log level is mixed and because of that most of the
  function now take `int` instead of `enum log_level`
- Environment variable `LOG_LEVEL` is now expected to be number and not string
- Message level enum renamed from `log_level` to `log_message_level` and values
  are shifted partially to negative
- `ASSERT` renamed to `assert` and `assert` to `log_assert` in `logc_assert.h`
- Utility macro `std_fatal` now provide result from statement

### Removed
- Output format fields `%l` and `%L`


### Fixed
- Linter and tests inclusion when they were not enabled by configuration script


## [0.1.0] - 2020-10-06
- Initial version of logc with limited support and unstable API
