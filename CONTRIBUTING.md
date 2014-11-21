How to contribute to this project
=================================

**Thank you in advance for your contributions!** Here are some guidelines to
follow.

- - - - -

Bug Reports
-----------

If you wish to file a bug report, please use the project's issue
tracker on GitHub. Ideally, your bug report will include the following
information:

* A brief summary of the problem, of just one or two sentences.

* Explicit steps to reproduce the problem. This should be complete
  and self-contained.

* What you expected to happen when you run the steps-to-reproduce.

* What you observed actually happening when you run the steps-to-reproduce.
  Include a shell console transcript.

* What version of the project you are using.

* What operating environment you are using the project in
  (OS, version, C compiler, etc.).

For your convenience, you can copy-and-paste this template, when going
to fill out a bug report. Remove the `[bracketed]` text, and replace it
with your own words and/or code.

```
SUMMARY
-------

[Insert summary here.]

STEPS TO REPRODDUCE
-------------------

1. [Insert step here.]
2. [Insert step here. Add as many steps as necessary.]

EXPECTED RESULT
---------------

[What did you expect to see?]

ACTUAL RESULT
-------------

[What actually happened?]

CONTEXT
-------

* Version: [Version of this project.]
* OS: [Your operating system, and version.]
* Compiler: [Your C compiler, and version.]

ADDITIONAL INFORMATION
----------------------

[Anything to add? If not, remove this section entirely.]
```

- - - - -

Feature Requests
----------------

If you have a feature request, then just use the same workflow as for
reporting bugs. It is okay to fill in `n/a` for the OS and Compiler fields.

- - - - -

Code Patches
------------

If you want to contribute code changes to the project, here is what
to do:

* *First and foremost* make sure that there is an issue (bug report
  or feature request) that covers the change you want to make. See
  above for details about filing an issue if there is not yet one.

* If you have not already done so, file a separate issue stating:

  ```
  For the record:

  * I hereby license all code I contribute to this project, under the
    stated terms of this project's license.
  * I hereby authorize the project's maintainers and their designated
    agents to act as my agent, for the purpose of copyright management
    of my contributions to this project.
  * All code I contribute to this project is my own original work.
  * I am not subject to any obligation that prevents me from contributing to
    this project under these terms.

  Signed,
  [your real name]
  ```

  **If you are not willing to do this, then your code will not be
  accepted into the project.** This is meant as a lightweight, yet still
  useful, way to prevent ne'er-do-wells from poisoning the project.

* Using the standard Git and GitHub workflow, fork the project, create
  one or more commits that address the issue in question, and submit the
  resulting commit history as a pull request.

  Please keep each commit self-consistent and self-contained. When in
  doubt, send a pull request that consists of a single squashed commit
  of all your changes, and make sure the commit message is a clean
  description of everything that you did.

* Limit each pull request to addressing just one issue.

* Test your patch. As of this writing, the most automated way to do that
  is to run `demo/run-all --runtime=tot --compiler=simple --build`, ensuring
  that no test fails.

  However, beware that these tests are far from complete, and it is also
  possible for them to produce incorrect output without a hard failure.
  Therefore, it is particularly important to (a) make sure your patch is
  covered by a test, and (b) inspect the output of that test "manually" for
  correctness.

  For bonus points, fix any related tests so that they do fail hard and loud
  instead of silently.
