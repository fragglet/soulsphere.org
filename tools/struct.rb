#!/usr/bin/env ruby
#
# Copyright(C) Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
#
# Generates read/write serialisation functions for C structures
#

class StructField
    attr_accessor :name, :type
    attr_accessor :ptr
    attr_accessor :enum
    attr_accessor :num_elements
    attr_accessor :orig

    def initialize(name, type)
        @name = name
        @type = type
        @ptr = false
        @enum = false
        @num_elements = 1
    end

    def to_s
        result = "#{@name}, type [#{@type}]"

        if @ptr
            result += "(pointer)"
        end
        if @num_elements != 1
            result += "(array of #{@num_elements} elements)"
        end

        result
    end
end

def get_struct_data(name)
    result = ''

    # strip down all headers and form into a single string

    IO.popen("cat *.h") do |io|
        io.each_line do |s|
            s = s.chomp
            s = s.sub(/\/\/.*/, '')
            s = s.sub(/^\s*#.*/, '')
            result += s
        end
    end

    # /* .. */ comments

    result = result.gsub(/\/\*.*?\*\//, '')

    # generate a list of all enum types

    $enums = {}

    result.gsub(/typedef\s+enum([^;]*);/) do
        $1 =~ /\W(\w*)$/
        $enums[$1] = true
    end

    # pick out the structure we want

    result.gsub(/\{\s*([^{}]*)\s*\}\s*#{name}/) do
        return $1
    end

    nil
end

def get_struct(name)
    data = get_struct_data(name)

    fielddata = data.split(/\s*;\s*/)

    fields = []

    fielddata.each do |s|
        num_elements = 1

        # compact whitespace

        s = s.gsub(/\s+/, ' ')

        orig = s

        # array ?

        if s =~ /(.*)\s*\[(.*)\]/
            s = $1
            num_elements = $2
        end

        # cut off the name

        s =~ /(.*\W)(\w+)$/

        s, name = $1, $2

        # get the typename

        s =~ /\s*(.*\S)\s*/
        typename = $1

        # create new field

        fld = StructField.new(name, typename)

        fld.num_elements = num_elements
        fld.orig = orig

        if typename =~ /\*$/
            fld.ptr = true
        end

        if $enums[typename]
            fld.enum = true
        end

        fields.push(fld)
    end

    fields
end

def gen_field_writer(field, varname)
    if field.ptr
        return "saveg_writep(#{varname});"
    elsif field.type == "int" || field.type == "boolean" || field.type == "fixed_t"
        return "saveg_write32(#{varname});"
    elsif field.enum
        return "saveg_write_enum(#{varname});"
    elsif field.type == "short"
        return "saveg_write16(#{varname});"
    elsif field.type == "char" || field.type == "byte"
        return "saveg_write8(#{varname});"
    else
        return "saveg_write_#{field.type}(&#{varname});"
    end
end

def gen_writer(name, fields)
    puts "void saveg_write_#{name}(#{name} *str)\n{"
    puts "\tint i;"

    fields.each do |f|
        puts
        puts "\t// #{f.orig};"

        if f.num_elements == 1
            puts "\t" + gen_field_writer(f, "str->#{f.name}")
        else
            puts "\tfor (i=0; i<#{f.num_elements}; ++i)"
            puts "\t{"
            puts "\t\t" + gen_field_writer(f, "str->#{f.name}[i]")
            puts "\t}"
        end
    end

    puts "}\n\n"
end

def gen_field_reader(field, varname)
    if field.ptr
        return "#{varname} = saveg_readp();"
    elsif field.type == "int" || field.type == "boolean" || field.type == "fixed_t"
        return "#{varname} = saveg_read32();"
    elsif field.enum
        return "#{varname} = saveg_read_enum();"
    elsif field.type == "short"
        return "#{varname} = saveg_read16();"
    elsif field.type == "char" || field.type == "byte"
        return "#{varname} = saveg_read8();"
    else
        return "saveg_read_#{field.type}(&#{varname});"
    end
end

def gen_reader(name, fields)
    puts "void saveg_read_#{name}(#{name} *str)\n{"
    puts "\tint i;"

    fields.each do |f|
        puts
        puts "\t// #{f.orig};"

        if f.num_elements == 1
            puts "\t" + gen_field_reader(f, "str->#{f.name}")
        else
            puts "\tfor (i=0; i<#{f.num_elements}; ++i)"
            puts "\t{"
            puts "\t\t" + gen_field_reader(f, "str->#{f.name}[i]")
            puts "\t}"
        end
    end

    puts "}\n\n"
end

strname = ARGV[0]

fields = get_struct(strname)

gen_reader(strname, fields)
gen_writer(strname, fields)

#fields.each { |f| puts f.to_s }

