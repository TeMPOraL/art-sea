#ifndef _lexical_cast
#define _lexical_casl
		//Lexical cast - convert between types based on their
        //text representation. Uses std::stringstream as a
        //conversion buffer.
        //Credit for boost::lexical_cast for the idea.
        template <typename Target, typename Source>
        inline Target lexical_cast(Source src)
        {
                std::stringstream buffer;       //buffer
                buffer.unsetf(std::ios::skipws);        //don`t skip white spaces
                buffer << src;    //write source argument to buffer

                Target retVal;  //return value

                buffer.unsetf(std::ios::skipws);        //don`t skip white spaces
                buffer >> retVal; //read returned value

                return retVal;  //return converted value
        }

        //lexical cast template specialization for astring
        template<>
        inline std::string lexical_cast(std::string src)
        {
                return src;
		}
#endif 
        