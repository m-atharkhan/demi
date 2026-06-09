# Demi Promotion & Growth Plan

A step-by-step checklist for growing the Demi community. Check off items as you complete them.

## Phase 1 — Project Polish (Week 1)

- [ ] Enable GitHub Discussions: Settings → General → Features → Discussions
- [ ] Add project to GitHub Topics: `compiler`, `virtual-machine`, `c-plus-plus-17`, `assembler`, `programming-language`
- [ ] Pin key issues with "good first issue" and "help wanted" labels
- [ ] Add a demo GIF/screencast to README showing `demi-engine -A hello.asm`
- [ ] Set up a project board (Projects tab) to show roadmap publicly
- [ ] Add social preview image (1200x630 PNG) to root as `social-preview.png`
- [ ] Verify all CI badges are green in README

## Phase 2 — Social Launch (Week 2)

- [ ] **Hacker News**: Post as "Show HN: Demi — a customizable virtual machine and compiler in C++17"
- [ ] **Reddit**: r/programming, r/cpp, r/compilers, r/ProgrammingLanguages
- [ ] **Twitter/X**: Thread showing the architecture + quick demo
- [ ] **Lobsters**: Submit with `compilers` and `performance` tags
- [ ] **Dev.to**: Write a "Building a Virtual Machine from Scratch in C++" article
- [ ] **Discord**: Join compiler-dev and programming-language communities, share in #showcase

## Phase 3 — Content (Ongoing)

- [ ] Blog post: "Why I'm Building a Custom VM" (personal motivation story)
- [ ] Blog post: "The Demi Instruction Set — Design Decisions"
- [ ] Video: 5-minute demo showing assembly → execution → debugging
- [ ] Tutorial series: "Writing Your First Demi Assembly Program"
- [ ] Comparison post: "Demi vs QBE vs Cranelift — different approaches to codegen"

## Phase 4 — Community Building (Ongoing)

- [ ] Respond to every issue and PR within 48 hours
- [ ] Thank first-time contributors publicly (Twitter, README)
- [ ] Add `CONTRIBUTORS.md` or use all-contributors bot
- [ ] Run a "Hacktoberfest" campaign in October
- [ ] Look for university students looking for compiler/VM projects
- [ ] Cross-promote with related projects (QBE, Cranelift, tinycompiler)

## Phase 5 — Sustained Growth

- [ ] Monthly "What's New in Demi" update posts
- [ ] Conference talks (CppCon, FOSDEM, local meetups)
- [ ] Add a `FUNDING.yml` with GitHub Sponsors
- [ ] Build an online playground (WebAssembly) so people can try without installing
- [ ] Create a Demi-specific website/landing page
- [ ] Write a paper about the architecture for arxiv.org

## Quick Commands

```bash
# Check community health
make promote

# Create a "good first issue" from the template
cp .github/ISSUE_TEMPLATE/good-first-issue.md /tmp/ && echo "Edit and post"

# Generate changelog for social posts
git log --oneline --since="2 weeks ago" --no-merges
```
