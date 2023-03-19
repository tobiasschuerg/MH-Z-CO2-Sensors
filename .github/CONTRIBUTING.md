# Contributing

Thank you for your interest in contributing to this project! Your contributions are highly appreciated. To ensure a smooth contribution process, please follow the guidelines below.

## Coding Style and Formatting

To maintain a consistent coding style throughout the project, please format your code using `clang-format` before submitting your changes. To do this, run the following command in the root directory of the project:

```bash
find . \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i -style=file {} \;
```

This command will apply the formatting rules defined in the .clang-format configuration file to all .cpp and .h files in the project.