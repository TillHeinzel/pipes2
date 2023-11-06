
function(getLocalIncludes retval currentPath)


	file(STRINGS "${currentPath}" lines REGEX "^#include \".*\"")
	
	cmake_path(GET currentPath PARENT_PATH currentFolder)

	foreach(line ${lines})
	  string(REGEX MATCH "\".*\"" includeStatement ${line})
	  string(REPLACE "\"" "" includeStatement ${includeStatement})
	  cmake_path(SET includePath NORMALIZE "${currentFolder}/${includeStatement}")
	  list(APPEND retvals ${includePath})
	endforeach()

	set(${retval} ${retvals} PARENT_SCOPE)
endfunction()

function(getAllFiles retval initialPath)
	set(fileList "${initialPath}")
	set(todoList "${initialPath}")

	list(LENGTH todoList todoLength)
	while(NOT (${todoLength} EQUAL 0))
		list(POP_FRONT todoList currentPath)

		set(includesFromFile "")
		getLocalIncludes(includesFromFile ${currentPath})

		foreach(file ${includesFromFile})
			if(NOT (${file} IN_LIST fileList))
				list(APPEND todoList ${file})
				list(APPEND fileList ${file})
			endif()
		endforeach()

		list(LENGTH todoList todoLength)
	endwhile()

	set(${retval} ${fileList} PARENT_SCOPE)
endfunction()

function(sortByNeededIncludes retval files)
	list(LENGTH files fileCount)

	while(NOT (fileCount EQUAL 0))
	
		foreach(file ${files})
			set(includesFromFile "")
			getLocalIncludes(includesFromFile ${file})

			foreach(include ${includesFromFile})
				if(${include} IN_LIST sortedFilesList)
					list(REMOVE_ITEM includesFromFile ${include})
				endif()
			endforeach()

			list(LENGTH includesFromFile unaddedDependencies)
			if(unaddedDependencies EQUAL 0)
				set(next ${file})
				break()
			endif()
		endforeach()

		list(REMOVE_ITEM files ${next})
		list(APPEND sortedFilesList ${next})
		
		math(EXPR fileCount "${fileCount} -1")
	endwhile()

	set(${retval} ${sortedFilesList} PARENT_SCOPE)
endfunction()

function(makeSingleHeader rootDir initialFile targetFileName desiredOrder)
	getAllFiles(allFiles "${rootDir}/${initialFile}")

	list(SORT allFiles COMPARE FILE_BASENAME)

	set(rest ${allFiles})

	foreach(orderingTerm ${desiredOrder})
		set(yesses ${rest})
		list(FILTER yesses INCLUDE REGEX "^${rootDir}/${orderingTerm}")
		list(APPEND presorted ${yesses})
		list(FILTER rest EXCLUDE REGEX "^${rootDir}/${orderingTerm}")
	endforeach()

	list(APPEND presorted ${rest})

	sortByNeededIncludes(sortedFiles "${presorted}")

	file(WRITE ${targetFileName} "#pragma once \n")

	foreach(header ${sortedFiles})	
		file(STRINGS ${header} lines)
	
		cmake_path(RELATIVE_PATH header BASE_DIRECTORY "${rootDir}" OUTPUT_VARIABLE printHeaderName)

		file(APPEND ${targetFileName} "\n")
		file(APPEND ${targetFileName} "//////////////////////////////////////////\n")
		file(APPEND ${targetFileName} "//${printHeaderName}\n")
		file(APPEND ${targetFileName} "//////////////////////////////////////////\n")
		file(APPEND ${targetFileName} "\n")

		foreach(line ${lines})
			string(FIND "${line}" "#include \"" includeLocation)
			string(FIND "${line}" "#pragma once" pragmaLocation)
			if((${includeLocation} EQUAL -1) AND (${pragmaLocation} EQUAL -1))
				file(APPEND ${targetFileName} "${line}\n")
			endif()
		endforeach()

	endforeach()
endfunction()

