#!/usr/bin/env ruby
#
# Self-modifying Brainfuck, by Simon Howard <fraggle@removethisbit.gmail.com>
#
# This behaves like normal brainfuck, except that the program is
# executed from the data tape.  The program text is inserted 
# immediately before the starting pointer position, ie. the following
# program prints the last character of the source file:
#
#        <.
#
# This allows for self-modifying code and introspection(reflection)
#

class Tape
    def initialize
        @data = "\0" * 1000
        @center = @data.length / 2
    end

    def [](index)
        @data[index+@center]
    end

    def []=(index, val)

        i = index + @center

        if i < 0 || i >= @data.length
            # resize the data array to be large enough

            new_size = @data.length

            loop do
                new_size *= 2
                test_index = index + (new_size / 2)
                if test_index >= 0 && test_index < new_size
                    # array is big enough now

                    break
                end
            end

            # generate the new array

            new_data = "\0" * new_size
            new_center = new_size / 2

            # copy old data into new array

            for j in 0...@data.length
                new_data[j-@center+new_center] = @data[j]
            end

            @data = new_data
            @center = new_center
        end

        @data[index+@center] = val & 0xff
    end
end

class Interpreter
    def initialize(data)
        @tape = Tape.new
        
        # copy the data into the tape

        for i in 0...data.length
            @tape[i-data.length] = data[i]
        end

        # program start point

        @entrypoint = -data.length
    end

    def char(c)
        sprintf("%c", c)
    end

    def call
        pc = @entrypoint
        ptr = 0

        while pc < 0
            case char(@tape[pc])
                when '.'
                    putc @tape[ptr]
                when ','
                    @tape[ptr] = STDIN.getc
                when '<'
                    ptr -= 1
                when '>'
                    ptr += 1
                when '-'
                    @tape[ptr] -= 1
                when '+'
                    @tape[ptr] += 1
                when '['
                    if @tape[ptr] == 0

                        # advance to end of loop

                        loop_level = 1

                        while loop_level > 0
                            pc += 1

                            loop_level += 1 if char(@tape[pc]) == '['
                            loop_level -= 1 if char(@tape[pc]) == ']'
                        end
                    end

                when ']'
                    # rewind to the start of the loop

                    loop_level = 1

                    while loop_level > 0
                        pc -= 1
                        loop_level -= 1 if char(@tape[pc]) == '['
                        loop_level += 1 if char(@tape[pc]) == ']'
                    end

                    pc -= 1
            end

            pc += 1
        end
    end
end

if ARGV.length <= 0
    puts "Usage: #{$0} <script filename>"
else
    File.open(ARGV[0]) do |file|
        data = file.readlines.join
        intr = Interpreter.new(data)
        intr.call
    end
end

