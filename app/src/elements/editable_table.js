import React from 'react';

// Components
import { useTable, useBlockLayout, useResizeColumns } from 'react-table';

import classNames from 'classnames';
import styled from 'styled-components'

import './styles/editable_table.css';
import './styles/various_inputs.css';

const Styles = styled.div`
    .table 
    {
        border-spacing: 0;
        width: 100%;
        height: 100%;

        .th {
            background-color: var(--theme-darker);
            font-weight: bold;
        }

        .th,
        .td {
            margin: 0;
            position: relative;
            padding: 1px;
            padding-left: 3px;
            border-bottom: 1px solid var(--foreground-color);
            border-right: 1px solid var(--foreground-color);

            :last-child {
            }

            .resizer {
                display: inline-block;
                background: white;
                width: 3px;
                height: 100%;
                position: absolute;
                right: 0;
                top: 0;
                transform: translateX(50%);
                z-index: 1;
                ${'' /* prevents from scrolling while dragging on touch devices */}
                touch-action:none;
        
                &.isResizing {
                    background: var(--theme-color);
                }
            }
        }
    }
`

const TableCell = ({
    value: initialValue,
    row: { index },
    column: { id },
    updateMyData, // This is a custom function that we supplied to our table instance
    removeData
}) => {
    // We need to keep and update the state of the cell normally
    const [value, setValue] = React.useState(initialValue)

    const onChange = e => {
        setValue(e.target.value)
    }

    const onDelete = e => {
        removeData(index);
    }

    // We'll only update the external data when the input is blurred
    const onBlur = () => {
        updateMyData(index, id, value)
    }

    // If the initialValue is changed external, sync it up with our state
    React.useEffect(() => {
        setValue(initialValue)
    }, [initialValue])

    if (id !== 'deleter')
        return <input className={classNames('minimalInput', 'tableCell')} value={value} onChange={onChange} onBlur={onBlur} />
    else
        return <div className="tableRemoveButton" onClick={onDelete}>{"\u274C"}</div>
}

function Table({columns, data, updateMyData, removeData, addLine}) {
    const defaultColumn = React.useMemo(
        () => ({
            minWidth: 5,
            width: 225,
            maxWidth: 4000,
            Cell: TableCell
        }),
        []
    )

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
        updateMyData,
        removeData,
        addLine
    },
        useBlockLayout,
        useResizeColumns,
        hooks => {
            hooks.visibleColumns.push(columns => {
                return [
                    {
                        id: 'deleter',
                        groupByBoundary: true,
                        width: 25,
                        Header: () => (
                            <div className="tableAddButton" onClick={() => {addLine()}}>{''}</div>
                        ),
                    },
                    ...columns
                ]
            })
        }
    )

    // Render the UI for your table
    return (
        <div {...getTableProps()} className="table">
            <div>
                {headerGroups.map(headerGroup => (
                    <div {...headerGroup.getHeaderGroupProps()} className="tr">
                        {headerGroup.headers.map(column => (
                            <div {...column.getHeaderProps()} className="th">
                                {column.render('Header')}
                                {/* Use column.getResizerProps to hook up the events correctly */}
                                <div
                                    {...column.getResizerProps()}
                                    className={`resizer ${column.isResizing ? 'isResizing' : ''}`}
                                />
                            </div>
                        ))}
                    </div>
                ))}
            </div>

            <div {...getTableBodyProps()}>
                {rows.map((row, i) => {
                    prepareRow(row)
                    return (
                        <div {...row.getRowProps()} className="tr">
                            {row.cells.map(cell => {
                                return (
                                    <div {...cell.getCellProps()} className="td">
                                        {cell.render('Cell')}
                                    </div>
                                )
                            })}
                        </div>
                    )
                })}
            </div>
        </div>
    )
}

class EditableTable extends React.Component
{
    state=
    {
        skipPageReset: false
    }

    constructor(props)
    {
        super(props);

        document.addEventListener('mousedown', this.defocus);
    }

    defocus = (event) =>
    {
        if (event === undefined || (this.inputRef && !this.inputRef.contains(event.target)))
            this.setState({editingIndex: -1});
    }

    setInputRef = (node) => 
    {
        this.inputRef = node;
    }

    updateData = (rowIndex, columnId, value) =>
    {
        this.props.onChange(rowIndex, columnId, value);
    }

    removeData = (rowIndex) =>
    {
        this.props.onDelete(rowIndex);
    }

    addLine = () => 
    {
        this.props.addLine();
    }

    render()
    {
        return (
            <div className={classNames("editableTable", this.props.className)}>
                <Styles>
                    <Table 
                        columns={this.props.columns} 
                        data={this.props.data} 
                        updateMyData={(...args) => {this.updateData(...args)}} 
                        removeData={(...args) => {this.removeData(...args)}}
                        addLine={this.addLine}
                    />
                </Styles>
            </div>
        );
    }
}

export default EditableTable;