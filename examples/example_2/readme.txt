Example project 2:
This example demonstrates how to share rules and their IDs between multiple generated parsing tables.
In this case there are three grammar files:
 -round.pika defines a recursive rule that uses round parenthesis
 -square.pika is similar but uses square braces instead
 -shared.pika defines some rules used by both

We can avoid generating two sets of IDs by invoking pmt_pika_prs_gen_cli three times:
 -once with $round as the start rule and all grammar files, NOT generating id strings or constants 
 -once with $square as the start rule and all grammar files, NOT generating id strings or constants 
 -once with BOTH $round and $square and all grammar files, ONLY generating id strings and constants

See scripts/gen_example_2.py for the exact commands