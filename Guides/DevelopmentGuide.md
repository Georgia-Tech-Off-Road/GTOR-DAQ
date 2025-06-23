# Introduction

**GitHub Repository:** [https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ](https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ) 

## Overview
This document is a document that details the git protocols and workflow of GTOR DAQ. If you do not have any experience with Git, please consult a senior team member (preferably someone who works more in software development) for a rundown before continuing to read this guide. The most important thing to remember when reading is that the guidance here is not ironclad, and is always, always, subject to **context**; namely the project you are working on and your level of experience. There are always cases where guidelines fail and should be broken, just make sure that you have a good reason to do so. There are very few ironclad **Rules (think of them as commandments)**, which I will highlight in **bold**. If you must commit a sin against our Git deities, make sure you have a damn good reason to do so. 

We will be using a modified version of [trunk-based development](https://www.atlassian.com/continuous-delivery/continuous-integration/trunk-based-development). The essential concept of trunk-based development is to eliminate long-running branches. Developers merge to the main branch (the “trunk”) whenever they have finished a stage of their current project. In our case, this ideally will range from every 1-5 development hrs. Again, this is highly subject to the work you are doing! More or less frequents merges into the trunk might be appropriate. 

To help keep everyone on the same track we be using a project based work system (along with trunk-based development). We do this to coordinate work among each (preventing conflicts and the like) as well as maintaining a clear focus for long and shorterm objectives.


 A project is a short(er) term effort to implement clearly defined objectives in a timely manner to work towards the longer term vision of a sa piece of software (either utility of library).  

## Goals
- Reduce merge conflict hell from long running branches
- Create a central repository of stable releases of tools
- Maintain a clear history of updates to tools and our software suite

#### Definition: Utility
*A utility is a singular piece of software that is accessible to end users and does not require other utilities to run in its simplest form (of course, library dependencies are fine). For example, our data translator is a utility, as is our website (even though it is more useful when receiving data from our data collection system on the car, another utility).*

#### Definition: Library
*Code that other utilities depend on to function but are not freestanding utilities on their own. For example, code calculating engine temperature from sensor output should be a library, as more than one utility requires this function (or likely would in the future).*

#### Definition: Software
*Encompasses both utility (software) and library (software).* 

# Project Stage 0: Proposal

Before you begin a project, document your ideas! For brainstorming an entirely new piece of software jot down your thoughts using SoftwareProposal.md form. Before begining a project on our software suite, use ProjectProposal.md. Both can be found in this folder. If you are envisioning a new piece of software and would like to begin work on it (in the form of a project) you will have to fill out both forms. This is to keep clear the distinction between the longterm goals of the software v.s the more limited short term goals of your initial project.  

#### Proposal Guidelines
1.	As much as you can, limit the scope of your project to one piece of software. This makes it easier for other teams to work on their own projects without worrying about merge conflicts with yours!
1.	Be conservative when laying out your timeline. Add 50% percent to your timeline deadlines from how long you think it will take. Especially on major projects, the team is relying on your project meeting deadlines (while still producing quality code!). 

You’ve also talked to other team members about this project, right? Good. Once you receive approval, you can continue to the next stage.

#### Definition: Project
A project is an endeavor to expand and improve DAQ’s software suite, guided by clear objectives set within a timeline. Each project should have its own Git branch.

## Project Stage 1: Creation

There are two different options from here, either you are creating a new utility / library (these both fall under the term software) or updating an existing piece of software.


#### Project Creation: New Software
Create a new branch from the main branch with the name of the project. All work done from here on for this project will be done in this project branch. It is a rule that every software have its own folder. For utilities, this will be a folder in the home directory, numbered with a two-digit code before (e.g. *07 Data Translation/*). Libraries must have their own folder in */00 Libraries*. Create your project folder, add it to the index with git add, and create a commit (with only this change!), titled Software Creation. 

e.x
```
git switch main
git branch "My-Project-Title"
git switch "My-Project-Title"
mkdir /"00 Libraries"/"[My New Libary]"
git add ./"00 Libraries"/"[My New Library]"
git commit -m "Software Creation"
```

**When in a project branch, only modify code within that project’s scope**. This prevents merge conflicts and a muddled commit history. If you need to work on multiple projects at once, you can always switch branches.


#### Project Creation: Existing Software
Create a new branch from the main branch with the name of the project. All work done from here on will be done in this project branch. 

e.x
```
git switch main
git branch "My-Project-Title"
git switch "My-Project-Title"
```

When in a project branch, only modify code within that project’s scope. This prevents merge conflicts and a muddled commit history. If you need to work on multiple projects at once, you can always switch branches.


# Project Stage 2: Experimentation
In this stage, you should work on research and experimentation to validate the viability of your intended projects. Because the experimentation phase moves fast and (somewhat) recklessly, this phase is easier done by one person. The goal is to have 80% of research and experimentation done by the end of this stage. If you anticipate significant changes in the project’s purpose, objectives, or timeline, consult with your teammates. At the end of this phase, present your findings to your teammates. If you decide to continue with the project, update your proposal form. **Leave the original text, but add your updates in red**.

#### Git Protocols
**All commits down in the stage should be titled Experiment:[Commit Title]**. Before you transition to the next phase, your commit history should be orderly. If I were you, this would mean making clean commits from the start, however you are free to do as you wish. Use `git pull` regularly to update your codebase with other teams’ work.

If you are developing solo, keep additions on your machine locally until they are somewhat polished. You do not have to push at any schedule but do push when possible so that teammates can view your progress!

If you must experiment as a team, figure out a synchronization strategy. One recommended approach is to divide responsibilities (to reduce merge conflicts) and regularly push and pull your local repositories. Avoid using `git push –-force` if at all possible. This is why code experimentation is not recommended to be done by a team. To clean up your commit history at the end, create a new branch from your project branch named Cleanup:[Insert Project Branch Name], clean your commit history there, and then merge the cleanup branch back into the project branch.

**Once you have a clean commit history, create a pull request of your project branch to main for review.**

# Project Stage 3: Development
At this point you should have clear objectives and a timeline for this project and have done most of the research necessary. It should be relatively straightforward programming from here! If you feel that you need more hands on deck, now is a good time to get more people involved. 

#### Git Protocols
**All commits in this phase should be titled Develop:[Commit Title]. Update your codebase regularly with** `git pull`. You should push to GitHub good commits, that means clear titles and commit messages. When you feel that you have a coherent assortment of commits, create a pull request of your project branch to main. This should be done every 30 min - 4 development hrs. The goal behind this is to present a clear, coherent contribution that can be reviewed efficiently. 

# Project Stage 4: Refinement
The goal of this phase is to make sure your code is ironclad, able to resist the strains of competition and mechanical engineers. Here are the 4 main things you may need to do in this phase:

1.	**Quality Assurance (“QA”) testing, debugging**
1.	Refactoring code, in-code documentation (ex. comments)
1.	Refinement, UI improvements etc.
1.	Out of code documentation, explainers, quick start guides, etc.

For QA on software-dependent code, use the same testing framework as found in other similar scenarios in the GitHub. If you need to use a new testing framework, consult with your team! To test code that is hardware-dependent (e.g. A sensor translation library), you will need to test manually or with a hybrid approach. 

#### Git Protocols
**All commits in the phase should be titled Refine:[Insert Commit Title]**. It should go without saying but regularly update your codebase with `git pull`. Everything already said about a clean commit history still applies. Pushing to GitHub should be done at a similar or even more frequent pace than development. Create a pull request when you have made a coherent (but not too large!) contribution. 

# Project Stage 5: Release
Finally, your project should be ready for release! It’s been battle tested and thoroughly documented (right?).  Hold your horses. Make sure that you’ve at least talked to your teammates about this. Depending on project size and impact, a meeting or presentation might be more appropriate.  Now that your code is ready, **go to the main branch of the release repository [insert link] and create a new branch titled [Project Name]/[Software Version]**. Add / update the relavant software folder(s) in that branch, replacing whatever was there previously. Name your folder(s) as follows, [Software Name] – [Software Version]. Version should be in MM.mm.bb format, where MM is the major version number, mm is minor version number, and bb is the bug / hotfix number. Once you have done all of this, create a pull request to the main branch of the release repo [insert link]. Once that pull request is approved, your code is officially in part of DAQ’s stable release code!

# Alternative: Hotfixes
Ideally all code pushed to release would be perfect and flawless, free of bugs and UI nuisances. Of course this is not realistic. There will be times when something slipped through, or you would like to make a minor code change or documentation update. In this case, the development overhead of the project framework does not make sense. For this we have hotfixes. Think of them as mini projects. To initiate a hotfix, fill out Hotfix.md. This defines the very specific action that you would like to fix. If you would like to work on two unrelated issues (even in the same project), you will need two hotfixes. After this, create a new branch from the branch titled Hotfix: [Hotfix title]. **All commits should be titled Hotfix:[Commit Title]**. Once your changes have been made, create a pull request to main, and then, if approved, add your code to release in the same way as a project. 


# Additional Responsibilities 
So far we have discussed how the project development workflow will go. This workflow relies heavily on teammates being involved and reviewing each other’s projects. Here we layout some additional responsibilities you have (in addition to working on your project) so that this workflow is possible.
1. You must be available to review other teamates pull requests. This system relies on peer review to produce good quality code. This does not mean that you must drop everything you are doing whenever a teammate submits a pull request, but coordinate with your team to get pull requests reviewed in a timely and efficient manner.
1. Talk with you teammates. This includes about your project, but also listen to them about their projects! In the same way that you need to review pull reqests to monitor code, talking to each other about your work helps to refine the broader design and concept.
1. Have fun! Seriously, DAQ aspires to be a quality code producing powerhouse, but it is also a college club. You are hereby commanded to have fun and let loose with the team, as well as produce quality code. 

# Future Goals

- Implement CI/CD system
