# Security Policy for demi

Last updated: 2025-11-17

Demi is a virtualized compiler that processes code and produces machine artifacts. Because of the nature of compilers and virtualized execution, security issues can have wide-reaching impact (remote code execution, sandbox escapes, supply-chain risks, or corrupted build artifacts). This document explains how to responsibly report security vulnerabilities and how we triage and respond.

## Reporting a vulnerability (preferred)

If you believe you've found a security vulnerability in demi, please report it privately so we can investigate and fix it before public disclosure.

Preferred methods (in priority order):

1. Use GitHub’s private Security Advisories for this repository:
   - https://github.com/bobrossrtx/demi/security/advisories/new
2. If you cannot use the GitHub advisory flow, open a private issue and mark it as "security" (do NOT include PoC or sensitive details in a public issue).
   - Start at: https://github.com/bobrossrtx/demi/issues/new and choose the appropriate privacy option.
3. If none of the above are possible, email the maintainer at the GitHub account listed on the repository (contact via your GitHub profile / direct message). We strongly prefer the GitHub Security Advisory flow because it supports private discussion and coordinated disclosure.

Do NOT post exploit details, proof-of-concept code, or any sensitive information in public issues, discussions, pull requests, or comments.

## What to include in a report

To help us triage quickly, please include as much of the following as possible while keeping the report private:

- A short, descriptive title.
- Affected demi version(s) or commit SHA(s).
- Affected platform(s) / architecture(s) (e.g., Linux x86_64, macOS arm64, Windows).
- Components affected (compiler frontend, virtual machine, runtime, build scripts, CI, packaging, third-party dependencies, etc.).
- Step-by-step reproduction instructions (minimal and copy-pastable if possible).
- Expected behavior vs. actual behavior.
- Proof-of-concept (PoC) or exploit code, if available — share privately.
- Any logs, crash dumps, stack traces, or sanitized artifacts.
- An assessment of impact (remote code execution, privilege escalation, data exfiltration, supply-chain compromise, DoS, etc.).
- Your contact information and preferred disclosure timeline.

If you must send attachments, prefer sharing them via private GitHub advisory or a secure transfer link.

## How we handle reports

- Acknowledgement: We will acknowledge receipt within 3 business days.
- Triage: Initial triage and risk assessment will normally be completed within 7 calendar days.
- Mitigation: For high- and critical-severity issues we will aim to provide a mitigation or patch within 30 days, or communicate a timeline and temporary mitigations if a full fix will take longer.
- Coordination: We will coordinate a timeline for public disclosure with the reporter. If a CVE is warranted, we will request/assign one and include it in the advisory/commit messages as appropriate.
- Communication: We will keep the reporter updated during triage and remediation. If we need help reproducing or validating a fix, we may ask for additional details or test cases.

These timelines are targets; complex issues may require longer investigation. If we are unable to meet a timeline, we'll communicate an updated plan.

## Severity guidance

We use common-sense severity categories to prioritize responses:

- Critical: remote code execution, sandbox escape, or supply-chain compromise with trivial exploitability.
- High: local privilege escalation, remote code execution requiring significant preconditions, disclosure of sensitive secrets.
- Medium: crashes or information exposure with limited impact or difficult exploitation.
- Low: minor information leakage, non-security bugs, or edge-case misconfigurations.

Final severity classification is determined by the maintainers during triage.

## Disclosure policy

We practice coordinated disclosure. We will not publicly disclose details until a fix or reasonable mitigation is available, unless:

- The reporter chooses to publicly disclose earlier, or
- A third party independently publishes working exploit details (in which case we will accelerate mitigation and disclosure).

We will credit the reporter in release notes or advisories, unless the reporter requests anonymity.

## Supported versions & scope

- When reporting, indicate the exact commit/branch or release tag you used.
- We support the latest stable release and up to two prior releases for security fixes by default. If an issue affects older, unsupported releases, we will evaluate backporting on a case-by-case basis.
- Scope includes code in this repository, official release artifacts, and first-party packaging scripts and CI config. Third-party libraries used by demi may have separate policies; we will coordinate as necessary.

## Supply-chain considerations

Because demi is a compiler and build-time tool, security issues can affect downstream users. When relevant, we will:

- Publish advisories with guidance for downstream projects.
- Provide patched releases and, where feasible, backports or mitigations.
- Coordinate with downstream package maintainers to ensure fixes propagate.

## Responsible disclosure expectations for reporters

- Keep the vulnerability report confidential while we investigate and remediate.
- Provide reasonable time for us to fix the issue before widespread public disclosure.
- Provide sufficient detail to reproduce or validate patches when requested.

We appreciate responsible disclosure and may offer public credit for reports following responsible coordination.

## Secure development & contribution practices

Contributors should follow our secure development guidance:

- Run tests and static analyzers locally where applicable.
- Avoid committing secrets or signing keys to the repository.
- Use code review for changes to components that affect parsing, sandboxing, execution, or code generation.
- When proposing major design changes that affect security (sandboxing model, JIT, code loading), open a design discussion and include security analysis and threat model.

## Reporting third-party dependency issues

If your report concerns a third-party dependency used by demi (e.g., a library in deps/ or submodule), please indicate that in the report. We will triage the issue and coordinate with upstream maintainers as appropriate.

## Acknowledgements and credits

We will acknowledge contributors who responsibly disclose vulnerabilities unless they request otherwise.

## Contact & legal

We will not pursue legal action against security researchers following responsible disclosure practices. Please do not attempt to access or exfiltrate private data during testing.

If you have questions about this policy or the reporting process, open a private GitHub security advisory or contact the maintainers through the repository’s contact options.

Thank you for helping keep demi secure.
