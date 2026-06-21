# Git hooks

Tracked hooks for this repo. Git does not use committed hooks automatically,
so each clone must point at this directory once:

```sh
git config core.hooksPath .githooks
```

## commit-msg

Strips any `Co-authored-by:` trailer mentioning Claude or Anthropic from commit
messages, so that automated tooling cannot list "Claude" as a GitHub
contributor by accident. Legitimate co-author trailers are kept.
