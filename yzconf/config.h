/*************************************************************************
	> File Name: config.h
	> Author: carl
	> Mail: carl_li510@163.com 
	> Created Time: 日 10/ 6 15:19:50 2019
 ************************************************************************/
#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace yzconf {

class   ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
    }
    virtual ~ConfigVarBase() {}
    std::string getName() const { return m_name; }
    std::string getDescription() const { return  m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString( const std::string& val) = 0;
private:
    std::string m_name;
    std::string m_description;
};




}


#endif //__CONFIG_H_
