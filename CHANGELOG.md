# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
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

### Changed
- Output format not-empty condition do not investigate sub-conditions, instead it
  considers them non-empty if they condition is fulfilled.
- Default formats now use `LOG_FP_LEVEL_NAME` for message type representation
- How message level and log level is mixed and because of that most of the
  function now take `int` instead of `enum log_level`
- Environment variable `LOG_LEVEL` is now expected to be number and not string
- Message level enum renamed from `log_level` to `log_message_level` and values
  are shifted partially to negative
- `ASSERT` renamed to `assert` and `assert` to `log_assert` in `logc_assert.h`

### Removed
- Output format fields `%l` and `%L`


### Fixed
- Linter and tests inclusion when they were not enabled by configuration script


## [0.1.0] - 2020-10-06
- Initial version of logc with limited support and unstable API
