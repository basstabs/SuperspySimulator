lines = IO.readlines ARGV[0]
replace = ARGV[1]
with = ARGV[2]
chance = ARGV[3].to_i

editedLines = Array.new

lines.each do |line|

    split = line.split " "

    split = split.map do |entry|

        if (entry == replace) && (chance > (rand 100))

            entry = with

        end

        entry

    end

    editedLines << (split.join " ")

end

File.open(ARGV[0], "w+") do |f|
  f.puts(editedLines)
end
