import '../App.css'
export default function Navbar() {
    return (
        <nav class="navbar">
            <ul class="nav-list">
                <li><a href="/"> Home </a></li>
                <li><a href="/test"> Test </a></li>
                <li><a href="/data">Race Data</a></li>
            </ul>
        </nav>
    );
}