# Contributing

# THIS DOCUMENT DOES NOT YET REFLECT THE CURRENT STATE OF THE PROJECT

# I WILL FIRST CONSULT WITH MY COLLEGE ON WHETHER IT IS A GOOD IDEA TO ALLOW CONTRIBUTIONS, BECAUSE IT MIGHT END UP BEING MY GRADUATION WORK

-- END OF NOTICE --

This is mostly a personal project for now, but contributions are welcome. Please read the following guidelines before
contributing.

## Getting Started

1. Fork the repository on GitHub
2. Clone the forked repository to your local machine. Make sure to include the `--recursive` flag to also clone the
   submodules.
3. Create a new branch for your changes
4. Make your changes, make sure to add tests for any new functionality.
5. Commit and push your changes to your forked repository. Please refer to
   the [commit message guidelines](#commit-message-guidelines)
   for information on how to format your commit messages.
6. Create a pull request from your forked repository to the `main` branch of the main repository.
7. Wait for a maintainer to review your pull request. If there are any issues, you will be notified via a comment on
   the pull request.
8. Once your pull request has been approved, it will be merged into the `main` branch.

## Code Style

The code style is enforced by clang-format. The configuration file is located at `.clang-format`.

## Issues

When choosing to work on the code, please make sure to **open an issue** first to discuss the change.
This will help avoid both wasted effort on invalid changes, and double work on changes that are already being worked
on.

If you can no longer work on the change yourself, please make sure to comment on the issue so that others may pick it.

When submitting a PR, please make sure to reference the issue that it is addressing.

**When opening an issue, please make clear whether you yourself are working on the change, or if you are requesting
the change. Issues are a valid way to request changes. It is not required for you to work on the change yourself.**

If you want to claim an issue (if it is not already claimed), please comment on the issue to let others know that you
intend to work on it.

## Welcome Changes

The following changes are listed "simple" and "Not Simple" not in terms of difficulty of implementation, but in terms of
difficulty of review. The more difficult a change is to review, the longer it will take to get merged.
Difficult-to-review changes include compiler extensions, diagnostics, etc. I.e., anything that requires discussion on
its inclusion.

One should adequately prove that the bug exists/behavior is non-standard.
To help with prioritization, please also make clear the impact of the bug/behavior.

### Notes and Definitions

- "Features" is defined as anything that is not a bug fix or a change to conform to the standard.
- Introducing new errors is considered a breaking change, and should be done with care and consideration.

### Simple

- Bug fixes
- Changing non-standard behavior to conform to the standard. **Missing standard-defined behavior is considered
  non-compliance, not a feature request.**
- Adding tests where you feel they are missing
- Documentation improvements
- Adding new warnings

### Not Simple

- Adding new features
- Adding new error diagnostics
- Adding new compiler extensions

### "It Depends" Changes

- Compiler codegen improvements, this depends on the potential impact on compile times and binary size. These kinds of
  changes must be **thoroughly** tested for unintended consequences.

## Commit Message Guidelines

**TL;DR**: Use [conventional commits](https://www.conventionalcommits.org/en/v1.0.0/).

Commit messages should be formatted as follows:

```
<type>(<optional scope>): <subject>
[optional body]
```

This requirement does not apply for `chore` commits, and is optional for pre-1.0.0 releases.
For breaking changes, you're to add an `!` before the `:` in the type/scope header.

For example:

```
feat!: add new feature
```

Scopes should be one of the following:

**TODO: Add scopes**

## File(name) Conventions

- Header files should be named with the `.h` extension.
- Source files should be named with the `.cpp` extension.
- Test files should be named with the `.test.cpp` extension.
- Test files should be located in the `tests` directory. Tested topics are to be grouped in a folder. An exception to
  this
  rule is when the test file is itself sufficient to cover the topic, then it may be a single file in the tests root
  directory. When an addition file is added to the topic, a folder should be created and the test file moved into it.
- Header and source files should be named with `snake_case`, unless they are only for the implementation of a class, in
  which case they should
  be named with `PascalCase`.

## Testing

For testing, we use [gtest](https://github.com/google/googletest).

To run the tests, there are batch or shell scripts provided for Windows or Unix-like systems.
