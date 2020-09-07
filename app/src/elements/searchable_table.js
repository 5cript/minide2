import React from 'react';

// Components
import Input from '@material-ui/core/Input';
import InputAdornment from '@material-ui/core/InputAdornment';
import SearchIcon from '@material-ui/icons/Search';
import InputLabel from '@material-ui/core/InputLabel';
import { useTable, useBlockLayout, useResizeColumns } from 'react-table';

// Other
import { withStyles } from '@material-ui/core/styles';
import classNames from 'classnames';
import styled from 'styled-components';

const SearchInput = withStyles({
    root: {
        height: '25px',
        minHeight: 0,
        margin: 0,
        color: 'var(--foreground-color)'
    }
})(Input);

const SearchLabel = withStyles({
    root: {
        margin: 0,
        color: 'var(--foreground-color)'
    }
})(InputLabel);

const Styles = styled.div`
  table {
    border-spacing: 0;

    tr {
      :last-child {
        td {
          border-bottom: 0;
        }
      }
    }

    th {
        background: var(--background-color-very-dark);
        text-align: left;
        padding-left: 8px;
        padding-right: 8px;
    }

    th,
    td {
      margin: 0;
      border-right: 1px solid var(--foreground-color);

      :last-child {
        border-right: 0;
      }
    }
  }
`

const CustomCell = ({
    value: initialValue,
    row: {index},
    column: {id},
    customRenderer
}) => {
    return customRenderer(initialValue, index)
}

const defaultColumn = {
    Cell: CustomCell
}

function Table({ columns, data, customRenderer }) {
    // Use the state and functions returned from useTable to build your UI
    const {
        getTableProps,
        getTableBodyProps,
        headerGroups,
        rows,
        prepareRow,
    } = useTable({
        columns,
        data,
        defaultColumn,
        customRenderer
    })

    // Render the UI for your table
    return (
        <table {...getTableProps()} style={{width: '100%', padding: '0px', margin: '0px'}}>
            <thead>
                {headerGroups.map(headerGroup => (
                    <tr {...headerGroup.getHeaderGroupProps()}>
                        {headerGroup.headers.map(column => (
                            <th {...column.getHeaderProps()}>{column.render('Header')}</th>
                        ))}
                    </tr>
                ))}
            </thead>
            <tbody {...getTableBodyProps()}>
                {rows.map((row, i) => {
                    prepareRow(row)
                    return (
                        <tr {...row.getRowProps()}>
                            {row.cells.map(cell => {
                                return <td {...cell.getCellProps()}>{cell.render('Cell')}</td>
                            })}
                        </tr>
                    )
                })}
            </tbody>
        </table>
    )
}

/**
 * props.elements = []
 * props.elementRenderer = (elem) => [] // components
 * props.elementFilter
 */
class SearchableTable extends React.Component
{
    state = {
        searchContent: ''
    }

    filteredData = () =>
    {
        const filtered = this.props.elements.filter(elem => {
            return this.props.elementFilter(elem, this.state.searchContent)
        })
        return filtered;
    }

    render() {
        return (
            <div className={classNames("searchableTable", this.props.className)}>
                <div className="searchableTableSearchBox">
                    <SearchInput
                        disableUnderline
                        id="searchableTableSearchInput"
                        startAdornment={
                            <InputAdornment position="start">
                                <SearchIcon style={{ color: 'white' }}/>
                            </InputAdornment>
                        }
                        value={this.state.searchContent}
                        onChange={value => {this.setState({searchContent: value.target.value})}}
                    />
                </div>
                <Styles>
                    <Table
                        columns={this.props.tableHeader.map(head => {
                            return {
                                Header: head.label,
                                accessor: head.accessor
                            }
                        })}
                        data={this.filteredData()}
                        customRenderer={this.props.elementRenderer}
                    >
                    </Table>
                </Styles>
            </div>
        )
    }

    /*
    render() {
        return (
            <div className="searchableTable">
                <SearchLabel htmlFor="input-with-icon-adornment">{this.props.dict.translate('$Search', 'search')}</SearchLabel>
                <div className="searchableTableSearchBox">
                    <SearchInput
                        disableUnderline
                        id="searchableTableSearchInput"
                        startAdornment={
                            <InputAdornment position="start">
                                <SearchIcon style={{ color: 'white' }}/>
                            </InputAdornment>
                        }
                        value={this.state.searchContent}
                        onChange={value => {this.setState({searchContent: value})}}
                    />
                </div>
                <table 
                    className="searchableTableContent"
                >
                    <tr className="searchableTableHead">
                        {this.props.tableHeader.map(head => {
                            return (
                                <th className="searchableTableHeadItem">{head.label}</th>
                            )
                        })}
                    </tr>
                    {this.props.elements.filter(elem => {
                        return this.props.elementFilter(elem)
                    }).map(element => {
                        return (
                            <tr key={this.props.elementKey(element)} className="searchableTableElement">
                                {this.props.elementRenderer(element)}
                            </tr>
                        )
                    })}
                </table>
            </div>
        )
    }
    */
}

export default SearchableTable;