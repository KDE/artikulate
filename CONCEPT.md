<!--
    Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: CC-BY-SA-4.0
-->

Concept for ArtiKulate (Pronunciation Self-Teaching)

Problem Statement and Goal
==========================
When learning a new language, usually the following skills must be accomplished: writing, reading, speaking, and understanding. For writing and reading a lot of materials and software programs (e.g., Parley for learning vocabulary) exist. But the important field of speaking and correct pronunciation of words and sentences is barely considered, yet, though it is one of the most important skills in real life. Hence this proposal is about an application that allows self-learning and improvement of pronunciation skills, which is usually only possible with the help of native (or at least very fluent) speakers that correct the learner.

Sketch
======
We part the functionality of the software into the part of the learner and the part of the teacher/native speaker who can provide teaching material.

Learner
=======
The learner select its learning language and downloads a pronunciation training for her language (Hot-New-Stuff or similar). The learning material is a set of words/phrases/sentences/paragraphs that are tagged by specific difficult pronunciation characters (e.g., "th" in English) and categorized by their difficulty (length of the text). Each text string has a sound file that contains the speaking of this phrase, recorded by a native speaker. The student then selects a difficulty level to start with and optionally a specific pronunciation he wants to improve. Then the program displays a list of phrases and with each the student can perform:
1. read the phrase
2. play the sound file
3. read the phrase herself and record that speaking
4. play her own recording and compare herself if this is similar to the native speaker's pronunciation
5. these steps can arbitrarily repeated and interleaved

Teacher/Native Speaker
======================
On the other side we need an editor functionality where a native speaker writes phrases, tags and categorize them, and record the speaking of them. Also it is necessary to allow an easy merge with existing files and an easy exchange interface to provide files to students (by Hot-New-Stuff or e.g. the Plasma Active Market).

Similar Programs
================
There exist similar commercial programs that are available for a few specific languages (e.g., "Son pour Son" for French, "....???" for English). But none of this programs is freely available and they lack of an availability of a broad scheme of teaching materials.

Open Problems
=============
Though the basic functionality of the software is quite simple, the main task is to form a userbase (at least for a very few languages at start) where we have native speakers who help to contribute recordings and who take care that the recordings (and if we possibly ship a default teaching material file for a language, that the recordings are of good quality and correct). So, besides the programming part the following task somehow must be solved:
1. provide initial teaching material for at least a few languages
2. integrate language teachers that actually use this software and by this have an incentive to provide/improve the pronunciation material
3. provide very simple ways to exchange and provide teaching material (it could also be a way to allow teaching material providers to provide their material by the Plasma Active Bodega market).
4. provide a Windows version to allow this software to be used in language learning centers (they use mainly Windows and cannot easily switch, since all other software operates on Windows)


TECHNICAL CONSIDERATIONS

Language Specification Files
----------------------------
For each language we need a specification file that states
  - TAGs for specific pronunciation patterns
  - links to further language education material
  - possible groups of tags (e.g. in French the group of all "nasal"-sounds)

Training Units
--------------
A unit consists of
  - specification of the language
  - list of (tagged and categorized) strings
  - according set of sound files
Further the strings are categorized by
  1. word
  2. expression
  3. sentence
  4. paragraph/sentences
The tags however, are specific for the current language. Though a basic set should be the phonetic alphabet.


COLOR CONCEPTS

Difficulty Levels:
word
- grün
  2DE86C
  RGB 45/232/108
expression
- blau
  327BFF
  RGB 50/123/255
sentence
- gelb
  FFF13F
  RGB 255/241/63
paragraph
- orange
  E85A02
  RGB 232/90/2
