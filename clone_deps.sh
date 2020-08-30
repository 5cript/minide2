useSsh=1

read -p "Are you 5cript / Use SSH?[y/N] " -n 1 -r
echo    # (optional) move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    useSsh=0
fi

if [[ $useSsh == 0 ]]; then
	git clone https://github.com/5cript/tab-complete.git
	git clone https://github.com/5cript/attender.git
	git clone https://github.com/5cript/automata.git
	git clone https://github.com/5cript/tab-complete.git
	git clone https://github.com/5cript/SimpleJSON.git
	git clone https://github.com/5cript/special-paths.git
	git clone https://github.com/5cript/debugger-interface.git
	git clone https://github.com/5cript/mplex.git
	git clone https://github.com/5cript/twisted-spirit.git
else
	git clone git@github.com:5cript/tab-complete.git
	git clone git@github.com:5cript/attender.git
	git clone git@github.com:5cript/automata.git
	git clone git@github.com:5cript/tab-complete.git
	git clone git@github.com:5cript/SimpleJSON.git
	git clone git@github.com:5cript/special-paths.git
	git clone git@github.com:5cript/debugger-interface.git
	git clone git@github.com:5cript/mplex.git
	git clone git@github.com:5cript/twisted-spirit.git
fi


# Always HTTPS
git clone https://github.com/ThomasMonkman/filewatch.git
git clone https://github.com/ThePhD/sol2.git
git clone https://github.com/lldb-tools/lldb-mi.git
git clone https://gitlab.com/eidheim/tiny-process-library.git